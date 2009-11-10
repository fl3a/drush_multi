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
 *   
 *   [name] => drupal 6.14
 *   
 *   [version] => 6.14
 *   
 *   [tag] => DRUPAL-6-14
 *   
 *   [version_major] => 6
 *   
 *   [version_patch] => 14
 *   
 *   [status] => published
 *   
 *   [release_link] => http://drupal.org/node/579476
 *   
 *   [download_link] => http://ftp.drupal.org/files/projects/drupal-6.14.tar.gz
 *   
 *   [date] => 1253130028
 *   
 *   [mdhash] => 014bb453a8d0adb0233c763e40d63859
 *   
 *   [filesize] => 1082687
 *   
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
 *   but also possible for versions before drupal 5 like drupal/4.7.x or image/4.7.x etc without error  
 * @param $project 
 * 	Name of the project. Default 'drupal' 
 * @return boolean
 *   TRUE if found on drupal.org else FALSE via drush_set_error() 
 */
function _multi_check_drupalmajorversion($core, $project = 'drupal') {
  
  $url = 'http://updates.drupal.org/release-history/'. $project . '/'. $core . '.x';
  if ($xml = @simplexml_load_file($url)) {
    if ($error = $xml->xpath('/error')) {
  	  return drush_set_error('MULTI_INVALID_PROJECT_OR_MAJOR_VERSION', 
  	    dt('Combination of project !project and major version !$core does not exist.',
  	      array('!project' => $project, '!core' => $major_version)
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
 * by searching for a settings.php which indicates a drupal-site
 * 
 * @param $uniq
 *   Suppress symlinks
 * @return $sites
 * 	 An array with folder names under sites/all or an empty array if no site detected
 */
function _multi_scan_sites($uniq = TRUE){
  
  /** @todo Check if settings.php is valid. Bootstrapping? */
  
  $sites = array();	
  $scan_result = drush_scan_directory(drush_get_context('DRUSH_DRUPAL_ROOT') . '/sites', 
  	'^settings.php$'
  );
  
  foreach($scan_result as $site ) {
    
    $tmp = array();
  	$tmp = split('/', ltrim($site->filename, '/'));
  	if ($uniq) {  	  
  	  $dir = $tmp;
  	  // unset last path part (settings.php), the site directory elements remain 
      unset($dir[count($dir)-1]);      
      $dir = implode('/', $dir);      
      // add a leading slash
      $dir = '/' . $dir;            
  	  if (is_link($dir)) {
  	    continue;
  	  }
  	}
  	// the site element
  	$sites[] = $tmp[count($tmp)-2];  		  	   		
  }
  return $sites;
}

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
function _multi_scan_links($dir = FALSE, $recurse = TRUE, $min_depth = 0, $depth = 0 ) {
  
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
        if (is_dir("$dir/$file") && $recurse) {
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
          }
        }        
      }
    }
    closedir($handle);
  }
  return $files;
} 

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
  $scan_result = drush_scan_directory(drush_get_context('DRUSH_DRUPAL_ROOT') . '/..', 
    '^drupal-' . drush_drupal_major_version(), array(), 0, FALSE 
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

/** @} */ /** End of defgroup filesystemm */
 
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

/** @} */ /** End of defgroup misc */ 
/** @} */ /** End of defgroup helper */