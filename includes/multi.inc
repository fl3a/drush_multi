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
 *   Core compatibility / drupal major version (5, 6, 7 and so forth)
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
 *   [name] => drupal 6.14   
 *   [version] => 6.14   
 *   [tag] => DRUPAL-6-14   
 *   [version_major] => 6   
 *   [version_patch] => 14   
 *   [status] => published   
 *   [release_link] => http://drupal.org/node/579476   
 *   [download_link] => http://ftp.drupal.org/files/projects/drupal-6.14.tar.gz   
 *   [date] => 1253130028   
 *   [mdhash] => 014bb453a8d0adb0233c763e40d63859   
 *   [filesize] => 1082687
 * @endcode   
 *  or the recommended version as float, if float is set to TRUE, 
 *  
 *  or FALSE via drush_set_error() if $core $project combination does 
 */
function _multi_recommended_version($core, $project = 'drupal', $float = FALSE, $path = FALSE) {
  
  if (!$path) {
    $path = 'http://updates.drupal.org/release-history/'. $project. '/'. $core . '.x';
  }    
  if ($xml = @simplexml_load_file($path)) {
    if ($error = $xml->xpath('/error')) {
  	  return drush_set_error('MULTI_COULD_NOT_LOAD_UPDATE_FILE', $error[0]);
    }
	else {
	  $recommended_major = $xml->xpath("/project/recommended_major");
	  $xpath_releases = "/project/releases/release[status='published'][version_major=" . (string)$recommended_major[0] . "]";
	  $releases = $xml->xpath($xpath_releases);
	  $release = (array)$releases[0];	  
	  unset($release['terms']);	   
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
 *   Core compatibility / drupal major version (5, 6, 7 and so forth)
 *   but also possible for versions before drupal 5 like drupal-4.7.x or image-4.7.x etc without error  
 * @param $project 
 * 	Name of the project. Default 'drupal' 
 * @return boolean
 *   TRUE if found on drupal.org else FALSE via drush_set_error() 
 */
function _multi_check_project($core, $project = 'drupal') {
  
  $url = 'http://updates.drupal.org/release-history/'. $project . '/'. $core . '.x';
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
 * Finds all sites in the drupal installation, 
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
  
  $sites = array();	
  $scan_result = drush_scan_directory(
    rtrim(drush_get_context('DRUSH_DRUPAL_ROOT')) . '/sites', '/^settings\.php$/'
  );
  
  foreach($scan_result as $site ) {
    
    $tmp = array();
  	$tmp = split('/', ltrim($site->filename, '/'));
  	if (!$symlinks) {  	  
  	  $dir = $tmp;
  	  // unset last path part (settings.php), it remains the site directory element 
      unset($dir[count($dir)-1]);
      // get directory string without a leading slash       
      $dir = implode('/', $dir);      
      // add a leading slash
      $dir = '/' . $dir;            
  	  if (is_link($dir)) {
  	    continue;
  	  }
  	}
  	// check site got a valid $db_url within settings.php
  	if (_multi_check_db_url($tmp[count($tmp)-2])) {
  	  // the site element
  	  $sites[] = $tmp[count($tmp)-2];
    }  		  	   		
  }
  return $sites;
}

/**
 * @defgroup symlinks 'Symlink functions'
 * Functions for handling and preserving symbolic links within the Drupal installation.
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
 *   An associative array of objects keyed basename  with
 *   "filename", "basename", and "target" (target of the symbolic link)
 *   members corresponding to found symbolic links 
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
          $files = array_merge( _multi_scan_links("$dir/$file", $recurse, $min_depth, $depth + 1), $files);
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

function _multi_preserve_symlinks() {
  
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
 */
function _multi_available_installations() {
  $scan_result = array();
  $scan_result = drush_scan_directory(
    rtrim(drush_get_context('DRUSH_DRUPAL_ROOT')) . '/..', 
    '/^drupal-' . drush_drupal_major_version().'/', array(), 0, FALSE 
  );
  foreach ($scan_result as $install) {
    $installations[] = $install->basename;        
  }  
  usort($installations, "strnatcmp");
  return $installations;  
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
  
  /** 
   * @todo check core version for url in critical message
   * (/?q=admin/logs/updates for 5.x, 
   * (/?q=admin/reports/updates for 6.x) 
   */
  
  if ($file = drush_get_option('file')) { 
     if (!is_file($file)) {
       $file = FALSE;
     }
  }
  else {
    $file = FALSE;
  }
  if ($recommended_version = _multi_recommended_version(drush_drupal_major_version(), 'drupal', FALSE, $file)) {
    if ((float) drush_drupal_version() < (float) $recommended_version['version']) {
      drush_print(  
        dt('DRUPAL ROOT CRITICAL - !version @ !drupal_root: drupal-!remcommended_version (!release-date) available, see !release_link for details.',
          array(
          	'!version'     => _multi_current_version(), 
         	'!drupal_root' => drush_get_context('DRUSH_DRUPAL_ROOT'),
          	'!remcommended_version' => $recommended_version['version'],
           	'!release-date' => date('Y-m-d\ H:i', $recommended_version['date']),
           	'!release_link' => $recommended_version['release_link'],       
         )
        )
      );      
      return MULTI_NAGIOS_CRITICAL;
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
 * EXPERIMENTAL
 * 
 * Monitores a single Drupal site for module and core updates
 * 
 * This function was heavily inspired by Markus Benning's
 * script at http://www.w3r3wolf.de/blog/wp-content/uploads/2009/08/check_drupal_update.php.gz
 * 
 * @note If there is a site at sites/default, this command runs on site basis, although you have only specified the Drupal installation.
 *  
 * @return MULTI_NAGIOS_OK(0) if monitored site is uptodate
 * or MULTI_NAGIOS_CRITICAL(2) if there are updates available
 */
function _multi_nagios_site() {
 
  /** @todo investigate why not all updateble modules are listed */
  /** @todo continue writing. */
  drush_bootstrap_max();
  $core = drush_drupal_major_version();

  drush_include_engine('update_info', 'drupal', NULL, DRUSH_BASE_PATH . '/commands/pm/update_info');
  
  $data = array();
  
  // Suppress output of  _pm_refresh()
  ob_start();
  //_pm_refresh();
  if ($core == 5) {
    $info = update_status_get_available();
    $data = update_status_calculate_project_data($info);
  }
  else if ($core = 6) {
    $available = update_get_available(TRUE);
    module_load_include('inc', 'update', 'update.compare');
    $data = update_calculate_project_data($available);
    
  }
  ob_end_clean();
  
  // Debug
  drush_print_r($info);
  drush_print_r($data);
  
  $modules = array();
  $updates = FALSE;

  foreach($data as $module) {
    
    if ($core == 5) {
      
      // Debug
      drush_print('--');
      drush_print('module:' . $module['name']);
      drush_print('version:' . $module['existing_version']);
      drush_print('recommended:' . $module['recommended']);
      drush_print('--');
      
      if($module['existing_version'] != $module['recommended']) {
        $updates = TRUE;
        $modules[] = $module['name'];    
        $updates_path = '/?q=admin/logs/updates';
      }      
    }
    else if($core == 6) {
      /** @todo check 7.x's _pm_get_update_info() result array */ 
      
      // Debug
      drush_print('--');
      drush_print('module:' . $module['name']);
      drush_print('version:' . $module['info']['version']);
      drush_print('recommended:' . $module['recommended']);
      drush_print('--');
      
      if($module['info']['version'] != $module['recommended']) {
        $updates = TRUE;
        $modules[] = $module['name'];    
        $updates_path = '/?q=admin/reports/updates';
      }
    }
  }

  if($updates) {
    drush_print(
      dt('DRUPAL SITE CRITICAL - !uri @ !site_root: there are updates for !modules, see http://!uri!updates_path for datails.',
        array(
      	  '!uri'      => drush_get_context('DRUSH_URI'),
          '!site_root'=> drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . drush_get_context('DRUSH_DRUPAL_SITE_ROOT'),
          '!modules'  => implode(',', $modules),   
          '!updates_path' => $updates_path,     
        )
      )
    );
    return MULTI_NAGIOS_CRITICAL;
  } 
  else {
    drush_print(
      dt('DRUPAL SITE OK - !uri @ !site_root: All modules are uptodate.',
        array(
      	  '!uri'      => drush_get_context('DRUSH_URI'),
          '!site_root'=> drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . drush_get_context('DRUSH_DRUPAL_SITE_ROOT'),
        )
      )
    );
    return MULTI_NAGIOS_OK;
  }
}
/** @} */ /** End of defgroup nagios */

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
  
  $db_spec = _drush_sql_get_spec_from_settings(
    rtrim(drush_get_context('DRUSH_DRUPAL_ROOT'),'/') . '/sites/' . $site . '/settings.php', ''
  );  
  
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