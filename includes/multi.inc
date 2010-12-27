<?php
// $Id$
/**
 * @file
 * Helper functions for multi commands
 */

/**
* @enum exit_status
* Numeric error status definitions
* Error code definitions for interpreting the current error status.
* @see drush_set_error(), drush_get_error(), drush_get_error_log(), drush_cmp_error()
* @{
*/

/** Status 0 */
define('MULTI_SUCCESS', 0);

/** Status 0 */
define('MULTI_NOTHING_TO_DO', 0);

/** Status 0 for nagios OK, see also _multi_nagios_site() and _multi_nagios_root() */
define('MULTI_NAGIOS_OK', 0);

/** Status 1 for nagios warning @see multi_nagios() */
define('MULTI_NAGIOS_WARNING', 1);

/** Status 2 for nagios critical @see multi_nagios() */
define('MULTI_NAGIOS_CRITICAL', 2);
/** @} */ /** End of defgroup exit_status */
 
/**
 * @defgroup helper_functions 'Helper functions'
 * Helper functions
 * @{  
 */

/**
 * @defgroup xml_xpath 'XML xpath functions'
 * Functions that uses the xml files at http://updates.drupal.org/release-history/project/core.x with xpath
 * @{
 *  
 */

/**
 * Get the recommended version of a specified project from a file or an url
 * 
 * @param $core 
 *   Core compatibility / drupal major version (5.x, 6.x, 7.x and so forth)
 *   but also possible for versions before drupal 5 like drupal/4.7.x or image/4.7.x etc without error
 * @param $project 
 *   Name of the project. Default 'drupal'
 * @param $float
 *   Modifies return value. 
 *   If TRUE it returns the recommended version of supplied project as float. Default FALSE
 * @param $path    
 *   Path to xml file which contains the release history.
 *   This could be an absolute path to file on your filesystem or an url.
 *   Default 'http://updates.drupal.org/release-history/' 
 *   combined with supplied $project,  '/', supplied  $core and '.x'.
 * @return
 *   An array with the following keys, we assume 6 was supplied for core, 'drupal' for project and v. 6.14 is recommended version:
 * @code
 * Array                                                      
 * (                                                          
 *   [name] => drupal 6.20                                  
 *   [version] => 6.20                                      
 *   [tag] => DRUPAL-6-20                                   
 *   [version_major] => 6                                   
 *   [version_patch] => 20                                  
 *   [status] => published                                  
 *   [release_link] => http://drupal.org/node/999422        
 *   [download_link] => http://ftp.drupal.org/files/projects/drupal-6.20.tar.gz
 *   [date] => 1292447789                                                      
 *   [mdhash] => a4f59401fbb3e20e3a03ac5fc11bd27c
 *   [filesize] => 1100246
 *   [files] => SimpleXMLElement Object
 *       (
 *           [file] => Array
 *               (
 *                   [0] => SimpleXMLElement Object
 *                       (
 *                           [url] => http://ftp.drupal.org/files/projects/drupal-6.20.tar.gz
 *                           [archive_type] => tar.gz
 *                           [md5] => a4f59401fbb3e20e3a03ac5fc11bd27c
 *                           [size] => 1100246
 *                           [filedate] => 1292447789
 *                       )
 *
 *
 *
 *                   [1] => SimpleXMLElement Object
 *                       (
 *                           [url] => http://ftp.drupal.org/files/projects/drupal-6.20.zip
 *                           [archive_type] => zip
 *                           [md5] => 06114b337ba9d682f44fa13d98ecfa46
 *                           [size] => 1285970
 *                           [filedate] => 1293228545
 *                       )
 *
 *               )
 *
 *       )
 *
 *   [terms] => SimpleXMLElement Object
 *       (
 *           [term] => SimpleXMLElement Object
 *               (
 *                   [name] => Release type
 *                   [value] => Bug fixes
 *               )
 *
 *       )
 *
 * )
 * @endcode   
 *  or the recommended version as float, if float is set to TRUE, 
 *  
 *  or FALSE via drush_set_error() if $core $project combination does 
 */
function _multi_recommended_version($core, $project = 'drupal', $float = FALSE, $path = FALSE) {
  
  if (!$path) {
    $path = 'http://updates.drupal.org/release-history/'. $project. '/'. $core;
  }    
  if ($xml = @simplexml_load_file($path)) {
    if ($error = $xml->xpath('/error')) {
  	  return drush_set_error('MULTI_COULD_NOT_LOAD_UPDATE_FILE', $error[0]);
    }
	else {
	  $recommended_major = $xml->xpath("/project/recommended_major");
      if (!empty($recommended_major)) {
	    $xpath_releases = "/project/releases/release[status='published'][version_major=" . (string)$recommended_major[0] . "]";
	    $releases = $xml->xpath($xpath_releases);
	    $release = (array)$releases[0];	    	   
	  }
	  else {
	    drush_log('No recommended version available, taking latest release', 'warning');
	    $xpath_releases = '/project/releases/release';
	    $releases = $xml->xpath($xpath_releases);
	    $release = $releases[0];
	  }	  
	  return (!$float) ? $release : $release['version'];
	}
  }
  else {
  	// We are not getting here since drupal.org always serves an XML response.
	return drush_set_error('MULTI_DOWNLOAD_FAILED', 
	  dt('Could not download project status information from !url', 
	   array('!url' => $path)
	  )
	);
  }  	
}

/**
 * Returns all released minor versions of the specified major version and project 
 * from drupal.org in form of ${PROJECT}'-'${MAJOR}.${MINOR}
 * or or FALSE via drush_set_error() if no release history was found for the requested project.  
 *  
 * @param $core 
 *   Core compatibility / drupal major version (5, 6, 7 and so forth)
 *   but also possible for versions before drupal 5 like drupal/4.7.x or image/4.7.x etc without error
 * @param $project 
 *   Name of the project, Default 'drupal'
 * @return array
 *   An ascending array that contains project and version in 
 *   form of ${PROJECT}'-'${MAJOR}.${MINOR} at each index or FALSE via drush_set_error()
 */ 
function _multi_released_versions($core, $project = 'drupal') {
  $url = 'http://updates.drupal.org/release-history/' .$project . '/'. $core . '.x';
  if ($xml = @simplexml_load_file($url)) {
    if ($error = $xml->xpath('/error')) {
  	  return drush_set_error('MULTI_COULD_NOT_LOAD_UPDATE_FILE', 
  	    $error[0]
  	  );
    }
	else {
	  $recommended_major = $xml->xpath("/project/recommended_major");
	  $xpath_releases = "/project/releases/release[status='published'][version_major=" . (string)$recommended_major[0] . "]";
	  $releases = $xml->xpath($xpath_releases);
	  foreach ($releases as $release) {
	    $released_versions[] = 'drupal-' . $release->version;	    
	  }
	  usort($released_versions, "strnatcmp");
	  return $released_versions;
	}
  }
  else {
  	// We are not getting here since drupal.org always serves an XML response.
	return drush_set_error('MULTI_DOWNLOAD_FAILED', 
	  dt('Could not download project status information from !url', 
	    array('!url' => $url))
	);
  }  	
}

/**
 * Check if the combination of the specified core compatibility and the specified project exists on drupal.org.
 *  
 * @param $core 
 *   Core compatibility / drupal major version (5.x, 6.x, 7.x and so forth)
 *   but also possible for versions before drupal 5 like drupal-4.7.x or image-4.7.x etc without error  
 * @param $project 
 * 	Name of the project. Default 'drupal' 
 * @return boolean
 *   TRUE if found on drupal.org else FALSE via drush_set_error() 
 */
function _multi_check_project($core, $project = 'drupal') {
  
  $url = 'http://updates.drupal.org/release-history/'. $project . '/'. $core;
  if ($xml = @simplexml_load_file($url)) {
    if ($error = $xml->xpath('/error')) {
  	  return drush_set_error('MULTI_INVALID_PROJECT_OR_MAJOR_VERSION', 
  	    dt('Combination of project !project and major version !$core does not exist.',
  	      array('!project' => $project, '!core' => $core)
  	    )
  	  );
    }
	else {
	  return TRUE;
	}
  }
  else {
  	// We are not getting here since drupal.org always serves an XML response.
	return drush_set_error('MULTI_DOWNLOAD_FAILED', 
	  dt('Could not download project status information from !url', 
	  array('!url' => $url))
	);
  }  	
}
 
/** @} */ /** End of defgroup xml_xpath */
 
/**
 * @defgroup filesystem 'Filesystem functions'
 * Functions that do search and scan operation on the local file system.
 * @{  
 */

/**
 * Finds all sites within the drupal installation (DRUSH_DRUPAL_ROOT), 
 * by searching for a settings.php which indicates a drupal-site.
 * and check for a valid $db_url for database connections.  
 * @see _multi_check_db_url()
 * 
 * @param boolean $symlinks
 *   Allows symlink site folders
 * @return $sites
 * 	 An array with folder names under sites/all 
 *   or an empty array if no site detected.
 */
function _multi_scan_sites($symlinks = TRUE){
  
  /** @todo Investigate the usage of drush`s core site detection via alias and consider removing this one */
  
  $sites = array();	
  $scan_result = drush_scan_directory(
    rtrim(drush_get_context('DRUSH_DRUPAL_ROOT')) . '/sites', '/^settings\.php$/'
  );
   
  foreach($scan_result as $site ) {
    
    $tmp = array();
  	$tmp = explode('/', ltrim($site->filename, '/'));
  	
  	if (!$symlinks) {  	  
  	  $dir = $tmp;
  	  // get directory string without a leading slash       
      $dir = implode('/', $dir);
      // add a leading slash
      $dir = '/' . $dir;            
  	  if (is_link($dir)) {
  	    continue;
  	  }
  	}  	
  	$sites[] = $tmp[count($tmp)-2];
  }
  return $sites;
}

/**
 * @defgroup symlinks 'Symlink functions'
 * Functions for handling and preserving symbolic links for @ref multi_drupalupdate_help.
 * @{  
 */

/**
 * Finds symbolic links (also hidden) in the drupal installation.
 * 
 * @param $dir
 *   The base directory for the scan, without trailing slash. Default ${drupal_root}
 * @param $recurse
 *   When TRUE, the directory scan will recurse the entire tree
 *   starting at the provided directory.
 * @param $min_depth
 *   Minimum depth of directories to return files from.
 * @param $depth
 *   Current depth of recursion. This parameter is only used internally and should not be passed.
 * @return
 *   An array of objects keyed on basename corresponding to the symbolic links found in $dir
 * @code
 * Array                                                                                                                                                         
 * (
 *       [sites] => stdClass Object
 *           (
 *               [filename] => /var/www/6.x/sites
 *               [basename] => sites
 *               [target] => ../6.x_sites/
 *               [dirname] => /var/www/drupal6
 *           )
 *       
 *       [backup] => stdClass Object
 *           (
 *               [filename] => /var/www/6.x/backup
 *               [basename] => backup
 *               [target] => ../6.x_backup/
 *               [dirname] => /var/www/drupal6
 *           )
 *           
 *       [profiles] => stdClass Object
 *           (
 *               [filename] => /var/www/6.x/profiles
 *               [basename] => profiles
 *               [target] => ../6.x_profiles/
 *               [dirname] => /var/www/drupal6
 *           )           
 * )
 * @endcode    
 */
function _multi_scan_symlinks($dir = FALSE, $recurse = TRUE, $min_depth = 0, $depth = 0 ) {
  
  if (!$dir) {
    $dir = (string) drush_get_context('DRUSH_DRUPAL_ROOT');        
  }
  // convert $dir to the direcory which it points to, if it's a link 
  $path_parts = pathinfo($dir);  
  $dir = (is_link($dir) && !realpath($dir)) ? $path_parts['dirname'] . '/' . readlink($dir) : $dir;
  
  $nomask = array('.', '..', 'CVS');
  $files = array();  

  if (is_dir($dir) && $handle = opendir($dir)) {
    while (FALSE !== ($file = readdir($handle))) {
      
      if (!in_array($file, $nomask)) { 
        if (is_dir("$dir/$file") && $recurse && !is_link("$dir/$file")) {
          // Give priority to files in this folder by merging them in after any subdirectory files.
          $files = array_merge( _multi_scan_symlinks("$dir/$file", $recurse, $min_depth, $depth + 1), $files);
        } 
        elseif ($depth >= $min_depth) {
          $filename = "$dir/$file";
          if (is_link($filename)) {          
            $basename = basename($file);
            $files[$basename] = new stdClass();
            $files[$basename]->filename = $filename;
            $files[$basename]->basename = $basename;
            $files[$basename]->target = readlink($filename); 
            $files[$basename]->dirname = dirname($filename);                      
          }
        }        
      }
    }
    closedir($handle);
  }
  return $files;
}

/**
 * Preserves all symbolic links of the current Drupal root
 * within the future Drupal root with a relative path.
 * 
 * If linkname already exists as file or directory within the provided path to $new_drupal_root, 
 * it will be renamed to ${linkname}-${datestamp}-drupalupdate to avoid loss.
 * 
 * @param $drupal_root Path to the current Drupal root
 * @param $new_drupal_root Path to the future Drupal root
 * @return FALSE via drush_set_error() if any error occurs
 */
function _multi_preserve_symlinks($drupal_root, $new_drupal_root) {
  
  // Change pwd to $new_drupal_root
  if (drush_op('chdir', realpath($new_drupal_root))) {
    
    // Search for symbolic links within the old installation ($drupal_root)
    $symlinks = array();
    $symlinks = _multi_scan_symlinks($drupal_root, TRUE);
    
    // traverse found symbolic links
    foreach ($symlinks as $symlink) {        
      
      // if linkname exist as file or directory, rename it 
      if (is_file($symlink->basename) || is_dir($symlink->basename)) {      
        if (!drush_op('rename', $symlink->basename, $symlink->basename . '-' . date('Y-m-d\TH:i') . '-drupalupdate')) {
          return drush_set_error('MULTI_COULD_NOT_RENAME',
            dt('Could not rename !existing to !backup', 
            array(
              	'!existing' => $symlink->basename, 
              	'!backup' =>  $symlink->basename . '-' . date('Y-m-d\TH:i') . '-drupalupdate'
              )
            )
          );   
        }     
      }
      // get $symlink_target as an relative path to our pwd ($new_drupal_root=
      $symlink_target = _multi_relative_path(realpath($symlink->target));
      
      // remove $drupal_root and trailing slash for handlind of symbolic links 
      // that are deeper than first level within $drupal_root   
      $symlink_dir = ltrim(str_replace($drupal_root, '', $symlink->dirname), '/');
      (!empty($symlink_dir)) ? $symlink_dir .= '/' : $symlink_dir;
      
      if (!drush_op('symlink', $symlink_target, $symlink_dir . $symlink->basename)) {
        return drush_set_error('MULTI_COULD_NOT_CREATE_SYMLINK',
          dt('Could not create symbolic link from !target into !pwd.',
            array(
              '!target' => rtrim($filename, '/'), 
              '!pwd' => getcwd()
            )
          )
        );  
      }         
    }
  }
  // couldn't chdir() to $new_drupal_root
  else { 
    return drush_set_error('DRUSH_SM_COULD_CHANGE_NEW_DRUPAL_ROOT', 
      dt('chdir(): Could not change to new Drupal root !new.',
        array('!new' => $new_drupal_root)
      )
    ); 	
  }  
}
/** @} */ /** End of defgroup symlinks */

/**
 * Returns all available drupal installations of your major release 
 * in form of 'drupal-'${MAJOR}.${MINOR} found in the parent directory of DRUPAL_ROOT 
 * sorted by usort with 'strnatcmp' callback
 * 
 * @return array 
 *    An ascending ordered array with drupal installation in form of 'drupal-'${MAJOR}.${MINOR},
 *    with at least the current installations found in the parent directory of DRUPAL_ROOT 
 *    or
 *    0 if none was found
 */
function _multi_available_installations() {
  $scan_result = array();
  $scan_result = drush_scan_directory(
    rtrim(drush_get_context('DRUSH_DRUPAL_ROOT')) . '/..', 
    '/^drupal-' . drush_drupal_major_version().'/', array(), 0, FALSE 
  );
  if (empty($scan_result)) {
    return 0;
  } 
  else {
    foreach ($scan_result as $install) {
      $installations[] = $install->basename;        
    }  
    usort($installations, "strnatcmp");
    return $installations;  
  }
}

/**
 * Return all previous drupal installations in form of 'drupal-'${MAJOR}.${MINOR} 
 * found in the parent directory of DRUPAL_ROOT in the filesystem or an empty array if not available.
 *  
 * @return array
 *   An ascending ordered array with the previous installations or FALSE if not available
 */
function _multi_previous_installations() {
  
  $installations = array();
  if(count($installations = _multi_available_installations()) != 1) {
    $size = array_keys($installations, _multi_current_version());
    if (sizeof($size) == 0) {
      return FALSE;
    }    
    // get chunks of previous(index 0) and newer(index 1, including current) releases
    $previous_versions = array_chunk($installations, $size[0]);
    if (sizeof($previous_versions[0]) != 0) {
      return $previous_versions[0];
    }
    else {
      return FALSE;  
    }
  }
  else {
    return FALSE;     
  }  
}


/**
 * Return the previous drupal installation in form of 'drupal-'${MAJOR}.${MINOR} 
 * found in the parent directory of DRUPAL_ROOT in the filesystem, else FALSE via drush_set_error
 *  
 * @return string
 *   the previous version if available or FALSE if not available
 */
function _multi_previous_installation() {
  
  $installations = array();
  $installations = _multi_available_installations();  
  if ($length = count($installations) != 1) { 	
    // Position of current version in array $installations
    $pos = array_keys($installations, _multi_current_version());
    if (bcsub($pos[0],1) != -1 ) {
      // value of previous in array $installations
      return $installations[bcsub($pos[0],1)];      
    }
    else {
      return FALSE;
    }    
  }
  else {
    return FALSE;  
  }		
}

/**
 * Return the relative path from the current working directory(getcwd()) to target.
 * 
 * based on http://de.php.net/ref.filesystem from http://iubito.free.fr
 * 
 * @param string 
 *   folder or file
 * @return string 
 *   the relative path from current working directory to $target as string with trailing '/'
 */
function _multi_relative_path($target) {
	
  $folder = $target;
  $target = getcwd();

  //Remove starting, ending, and double / in paths
  $target = trim($target,'/');
  $folder = trim($folder,'/');
  while (substr_count($target, '//')	) $target = str_replace('//', '/', $target);
  while (substr_count($folder, '//')) $folder = str_replace('//', '/', $folder);

  //create arrays
  $arr1 = explode('/', $target);
  if ($arr1 == array('')) $arr1 = array();
  $arr2 = explode('/', $folder);
  if ($arr2 == array('')) $arr2 = array();
  $size1 = count($arr1);
  $size2 = count($arr2);

  //now the hard part :-p
  $path='';
  for($i=0; $i<min($size1,$size2); $i++) {
    if ($arr1[$i] == $arr2[$i]) continue;
      else $path = '../'.$path.$arr2[$i].'/';
    }
    if ($size1 > $size2)
      for ($i = $size2; $i < $size1; $i++)
        $path = '../'.$path;
      else if ($size2 > $size1)
        for ($i = $size1; $i < $size2; $i++)
          $path .= $arr2[$i].'/';

  return $path;
}
/** @} */ /** End of defgroup filesystem */

/**
 * @defgroup nagios 'Nagios functions'
 * Functions that deals with Nagios
 * @{  
 */

/**
 * Monitores a Drupal installation for core updates
 * @return MULTI_NAGIOS_OK(0) if monitored DRUPAL_ROOT is uptodate
 * or MULTI_NAGIOS_CRITICAL(2) if there are updates available
 */
function _multi_nagios_root() {
  
  if ($file = drush_get_option('file')) { 
     if (!is_file($file)) {
       $file = FALSE;
     }
  }
  else {
    $file = FALSE;
  }
  if ($recommended_version = _multi_recommended_version(drush_drupal_major_version(). '.x', 'drupal', FALSE, $file)) {
   
    if ((float) drush_drupal_version() < (float) $recommended_version['version']) {
      
      $update_type = $recommended_version['terms']->term->value; 
      if($update_type == 'Security update') {
        $status_signal = dt('DRUPAL ROOT CRITICAL');
        $status_code = MULTI_NAGIOS_CRITICAL;
      }
      else {
        $status_signal = dt('DRUPAL ROOT WARNING');
        $status_code = MULTI_NAGIOS_WARNING;
      }      
            
      drush_print(  
        dt('!status_signal - !version @ !drupal_root: drupal-!remcommended_version (!update_type) from !release-date available, see !release_link for details.',
          array(
            '!status_signal' => $status_signal, 
          	'!version'     => _multi_current_version(), 
         	'!drupal_root' => drush_get_context('DRUSH_DRUPAL_ROOT'),
          	'!remcommended_version' => $recommended_version['version'],
            '!update_type' => $update_type,
           	'!release-date' => date('Y-m-d\ H:i', $recommended_version['date']),
           	'!release_link' => $recommended_version['release_link'],       
          )
        )
      );
      return $status_code;          
    }
    else {
      drush_print(
      	dt('DRUPAL ROOT OK - !version @ !drupal_root is uptodate.',
          array(
      		'!version' => _multi_current_version(), 
         	'!drupal_root' => drush_get_context('DRUSH_DRUPAL_ROOT'),         	
          )
        )
      );
      return MULTI_NAGIOS_OK;           
    }   
  }
  else {
    return $recommended_version; // FALSE if _multi_recommended_version() sets an error
  }
}

/**
 * Monitores a single Drupal site for module and core updates * 
 *  
 * @return 
 * MULTI_NAGIOS_OK (0) if monitored site is uptodate
 * or MULTI_NAGIOS_WARNING (1) if there are updates or unsupported modules available
 * or MULTI_NAGIOS_CRITICAL(2) if there are security updates available
 */
function _multi_nagios_site() {
  
  drush_include_engine('update_info', 'drupal', NULL, DRUSH_BASE_PATH . '/commands/pm/update_info');
  include(DRUSH_BASE_PATH . '/commands/pm/updatecode.pm.inc');
  
  $updatable = FALSE;
  $updates = $security_updates = $not_supported = array();
  
  drush_bootstrap_max();
    
  // Get update status information.
  ob_start();
  $projects = _pm_get_update_info();
  ob_end_clean();
  
  // Get name and type of update
  foreach($projects as $project) {
    switch (pm_update_filter($project)) {
      case dt('Update available') :
        $updates[] = $project['name'];
        $updatable = TRUE;
        break;
      case dt('SECURITY UPDATE available'):
        $security_updates[] = $project['name'];
        $updatable = TRUE;
        break;
      case dt('Installed version not supported'):
        $not_supported[] = $project['name'];
        $updatable = TRUE;
        break;
    }    
  }
  
  // Get uri for Available updates
  switch (drush_drupal_major_version()) {
    case 5:
      $updates_path = '/?q=admin/logs/updates';
      break;
    case 6:
    case 7:
      $updates_path = '/?q=admin/reports/updates';
      break;
  }
  
  // Build message and status
  if($updatable) {
    // CRITICAL / SECURITY UPDATES
    if (!empty($security_updates)) {
      $status_msg = dt('DRUPAL SITE CRITICAL - !uri @ !site_root: ', 
        array(
        '!uri'      => drush_get_context('DRUSH_URI'),
        '!site_root'=> drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . drush_get_context('DRUSH_DRUPAL_SITE_ROOT'),            
        )
      );
      $status_msg .= dt('SECURITY UPDATES available for !modules', 
        array(
          '!modules' => implode(',', $security_updates),
        )
      );
      if (!empty($updates)) {
        $status_msg .= dt(', Updates available for !modules', 
          array(
         	'!modules' => implode(',', $updates)
          )
        );          
      }
      if (!empty($not_supported)) {
        $status_msg .= dt(', Installed version not supported: !modules',
          array(
            '!module' => implode(',', $not_supported)
          )
        );  
      }
      $status_msg .= dt(', see http://!update_uri for details.', 
        array(
          '!update_uri' => drush_get_context('DRUSH_URI') . $updates_path          
        )
      );
      $status_code = MULTI_NAGIOS_CRITICAL;
    }
    // WARNING / Updates or Unsupported modules
    else if (!empty($updatable) || !empty($not_supported)) {
      $status_msg = dt('DRUPAL SITE WARNING - !uri @ !site_root: ', 
        array(
        '!uri'      => drush_get_context('DRUSH_URI'),
        '!site_root'=> drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . drush_get_context('DRUSH_DRUPAL_SITE_ROOT'),            
        )
      );
      if (!empty($updates)) {
        $status_msg .= dt('Updates available for !modules', 
          array(
         	'!modules' => implode(',', $updates)
          )
        );          
      }
      if (!empty($not_supported)) {
        if (!empty($updates)) {
          $status_msg .= ', ';
        }
        $status_msg .= dt('Installed version not supported: !modules',
          array(
            '!module' => implode(',', $not_supported)
          )
        );  
      }
      $status_msg .= dt(', see http://!update_uri for details.', 
        array(
          '!update_uri' => drush_get_context('DRUSH_URI') . $updates_path          
        )
      );
      $status_code = MULTI_NAGIOS_WARNING;
    }
  }
  else {  
    // OK / No updates
    $status_msg =  dt('DRUPAL SITE OK - !uri @ !site_root: Everything is uptodate.',
      array(
  	    '!uri'      => drush_get_context('DRUSH_URI'),
        '!site_root'=> drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . drush_get_context('DRUSH_DRUPAL_SITE_ROOT'),
      )
    );
    $status_code = MULTI_NAGIOS_OK;
  }  
    
  // Print message and exit with status
  drush_print($status_msg);  
  return $status_code;  
}
/** @} */ /** End of defgroup nagios */

/**
 * @defgroup sql-dump 'SQL dump functions'
 * SQL dump functions. 
 * @{
 */

/**
 * Helper function for a full sql-dump 
 *  
 * @param $db_spec
 *   An array containing the db spec	
 *   
 * @param $site
 *   A string containing the site name
 *   
 * @param $datetime
 *   A string containing the datetime
 *   
 * @param $comment
 *   A string containing the comment
 *   
 * @param $target
 *   A string containg the path where the dumps should be stored
 *   
 * @see drush_multi_sql_dump()  
 */
function _multi_sql_dump_full($db_spec = array(), $site = NULL, $datetime = NULL, $comment = NULL, $target = NULL) {
  
  // Building result-file name 
  $result_file = $target . '/' . $site . $datetime . $comment . '.sql';
  
  // set --result-file argument for drush_sql_dump()
  drush_set_option('result-file', $result_file);
    
  $exec = drush_sql_dump($db_spec);        
  if (drush_op('system', $exec) == 0) {
    drush_log(
      dt('SQL dump for site !site, database !database saved in !file.', 
        array(
          '!site'     => $site,
          '!database' => $db_spec['database'],
          '!file'     => $result_file
        )
      ),
      'success'        
    );      
    if (drush_get_option('bzip2')) {
      // delete $result_file if it already exists 
      if (is_file($result_file . '.bz2')){
        drush_shell_exec('rm '. $result_file . '.bz2');
        drush_log(
          dt('Overwriting existing file !file',
            array('!file' => $result_file . '.bz2')
          ),
          'success'
        );               
      }        
      if (drush_shell_exec("bzip2 \"$result_file\"")) {
        drush_log(dt('Compressed !file using bzip2.', 
          array('!file' => $result_file)),
        	'success'        
        );
      }                
    } 		  
  }    
}

/**
 * Helper function for a table wise sql-dump (one file for each table)
 * Only works with mysql
 *  
 * @param $db_spec
 *   An array containing the db spec	
 *   
 * @param $site
 *   A string containing the site name
 *   
 * @param $datetime
 *   A string containing the datetime
 *   
 * @param $comment
 *   A string containing the comment
 *   
 * @param $target
 *   A string containg the path where the dumps should be stored
 *   
 * @see drush_multi_sql_dump()  
 */
function _multi_sql_dump_tables($db_spec = array(), $site = NULL, $datetime = NULL, $comment = NULL, $target = NULL) {
 
  // get available tables from current database (site)      
  $exec = 'echo "SHOW TABLES"|';
  $exec.= 'mysql -u' . $db_spec['username'];
  $exec.= ' -p'. $db_spec['password'];
  $exec.= ' '. $db_spec['database'];
  $output = shell_exec($exec);
  
  $tables = array();
  $tables = explode("\n", $output);
  // remove "Tables_in_database" from array
  unset($tables[0]);
  // remove last element which contains the databasename
  unset($tables[sizeof($tables)]);

   // iterate tables
  foreach($tables as $table) {
  
    // Building result-file name 
    $result_file = $target . '/' . $site . '.' . $table . $datetime . $comment . '.sql';
           
    // build mysqldump statement
    $exec = 'mysqldump -u' . $db_spec['username'];
    $exec.= ' -p'. $db_spec['password'];
    $exec.= ' '. $db_spec['database'] .' ' . $table;
    //drush_print($exec . '>' . $result_file);
    if (drush_shell_exec($exec . '>' . $result_file)) {
      drush_log(
        dt('SQL dump for site !site, datebase !database, table !table saved in !file.', 
          array(
            '!site'      => $site,
            '!database'  => $db_spec['database'],
            '!table'     => $table, 
            '!file'      => $result_file
          )
        ),
      	'success'        
      );
      if (drush_get_option('bzip2')) {
        // delete $result_file if it already exists 
        if (is_file($result_file . '.bz2')){
          drush_shell_exec('rm '. $result_file . '.bz2');
          drush_log(
            dt('Overwriting existing file !file',
              array('!file' => $result_file . '.bz2')
            ),
            'success'
          );               
        }        
        if (drush_shell_exec("bzip2 \"$result_file\"")) {
          drush_log(dt('Compressed !file using bzip2.', 
            array('!file' => $result_file)),
          	'success'        
          );
        }                
      } 		  
    }                 
  } 
}

/** @} */ /** End of defgroup sql-dump */

/**
 * @defgroup misc 'Miscellaneous functions'
 * Miscellaneous functions. 
 * @{
 */

/**
 * Return the current drupal version in form of 'drupal-'${MAJOR}.${MINOR}
 * @return string
 *   Drupals current version
 */
function _multi_current_version() {
  return 'drupal-' . drush_drupal_version();
}

/**
 * Check if the supplied site exists within the installation
 * @param $site
 * @return boolean
 * Return TRUE if the site exists within the installation else FALSE
 */
function _multi_check_site($site) {
  
  $sites = array();
  $sites = _multi_scan_sites();
  if (in_array($site, $sites)) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Check the database url in settings.php of the supplied site by trying to establish a connection
 * 
 * @param $site
 * The site to check
 * @return TRUE if the connection was established succesfull else FALSE
 */
function _multi_check_db_url($site) {
  /** @todo more tests */
  $db_spec = array();  
  $db_spec = _drush_sql_get_db_spec();
  
  // connect via database specific functions
  switch ($db_spec['driver']) {
    case 'mysql':
      $link = @mysql_connect(
        $db_spec['host'] . ':' . $db_spec['port'],
        $db_spec['username'],
        $db_spec['password'],
        $db_spec['database']        
      );
      break;
    case 'mysqli':
      /** @todo check mysqli_connect(), not tested */      
      $link = mysqli_connect(
        $db_spec['host'],
        $db_spec['username'],
        $db_spec['password'],
        $db_spec['database'],
        $db_spec['port']
      );
      break;
    case 'pgsql':
      /** @todo check pg_connect(), not tested */
      // creating postgresql connection string
      $connection_string  = 'host=' . $db_spec['host'] .' ';
      $connection_string .= 'user=' . $db_spec['user'] .' ';
      $connection_string .= 'password=' . $db_spec['password'] .' ';
      $connection_string .= 'dbname=' . $db_spec['database'];
      if (!empty($db_spec['port'])) {
        $connection_string .= ' port=' . $db_spec['port'];
      }      
      $link = pg_connect($connection_string);
      break;
  }
  
  // check the database connection
  if (!$link) {
    return FALSE;
  }
  else {
    // close the established database connection
    switch ($db_spec['driver']) {
      case 'mysql':
        mysql_close($link);
        break;
      case 'mysqli':
        mysqli_close($link);
        break;
      case 'pgsql':
        pg_close($link);
        break;           
    }
    return TRUE;   
  }   
}


/** @} */ /** End of defgroup misc */ 
/** @} */ /** End of defgroup helper */