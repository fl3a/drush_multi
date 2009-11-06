<?php
// $Id$

include 'includes/multi.inc';

/**
 * @file
 * The multi implementation.   
 */

/**
 * @defgroup drush_hooks 'Drush hooks'
 * Implementation of drush hooks
 * @{  
 */

/**
 * Implementation of hook_drush_help().
 * @param
 *   A string with the help section (prepend with 'drush:')
 * @return
 *   A string with the help text for your command.
 */
function multi_drush_help($section) {
  switch ($section) {
    case 'drush:multi status':
      return dt("Status of your multisite installation.");    
    case 'drush:multi exec':
      return dt('Execute a drush command for all sites in your multisite installation.');
    case 'drush:multi drupalupdate':
      return dt('Run a drupal minor update on your multisite installation.');
    case 'drush:multi site':
      return dt('Create a new site in your your multisite installation within a files, modules and themes directory.');
    case 'drush:multi create':
      return dt('Create a multisite installation.');
    case 'drush:multi sql dump':
      return dt('Execute \'sql dump\' for all sites in your your multisite installation. The default pattern for the result-file is $uri . \'_\' . date(\'Y-m-d\TH:i\').sql, where $uri is the foldername of a site.');
    case 'drush:multi nagios':
      return 'For the use as nagios plugin. Print a message and returns with exit status 2 (Critical) if there is a drupal update available.';      
  }
}

/**
 * Implementation of hook_drush_command().
 * @See drush_parse_command() for a list of recognized keys.
 * @return
 *   An associative array describing your command(s).
 */
function multi_drush_command() {
  $items = array();

  $items['multi status'] = array(
    'callback' => 'multi_status',
    'aliases' => array('mst'),
    'description' => 'Status of your drupal multisite installation',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT, // Find a valid Drupal root.
    'examples' => array(
      'drush -r /path/to/drupal multi status' => 'Displays the status.',
    ),      	
  );
  $items['multi nagios'] = array(
    'callback' => 'multi_nagios',
    'aliases' => array('mna', 'nagios'), 
    'description' => 'Message and exit code for nagios.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'examples' => array(
      'drush -r /path/to/drupal multi nagios' =>
      "We assume that drupal-6.14 is the recommended release.\n" . 
      "With an older version, eg. drupal-6.13 we get:\n" .
      "DRUPAL 6.x CRITICAL: drupal-6.13 @ /var/www/drupal-6.13\n" .
      " - drupal-6.14 (2009-09-16 21:40) available, see http://drupal.org/node/579476 for details.\n" .
      "with exit status 2\n" .
      "Or if we are running the recommended version:\n" .
      "'DRUPAL 6.x OK: drupal-6.14 @ /path/to/drupal/6.x is uptodate.'\n" .
      "with exit status 0.",
      'drush -r /path/to/drupal multi nagios --file=/path/to/file' => 'Same as above with a specified file.'
    ), 
    'options' => array(
      '--file' => 'Path to release information file, this could be a (temp) file  to avoid nrpe-socket timeouts. It is also possible to supply path via URI, like http://updates.drupal.org/release-history/drupal/6.x.'
    ),    
  );
  $items['multi moo'] = array(
    'aliases' => array('moo'),
    'callback' => 'multi_moo',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );
  $items['multi exec'] = array(
    'aliases' => array('mex'),
    'description' => 'Run a drush command on all sites in your multisite installation. Implemented drush commands are: \'cache clear\', \'cron\', \'update\', \'updatecode\', \'updatedb\', \'watchdog delete\', \'watchdog show\' and \'refresh\'.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT, // Find a valid Drupal root.
    'examples' => array(
      'drush -r /path/to/drupal multi exec cron' => 'Run all cron hooks on all sites in your multisite installation.',
      'drush -r /path/to/drupal multi exec \'watchdog show\' --option=\'--limit=30\' --argument=php'  => 'Shows the 30 newest log entries with facility \'php\' for all sites.',
      'drush -r /path/to/drupal multi exec updatedb'  => 'Execute the update.php process for all sites.',
    ),     
    'options' => array(      
      '--option'     => 'Option to pass to drush command. e.g. \'--limit=20\' for \'watchdog show\'. The number of messages to show. Optional.',
      '--argument'		=> 'Argument to pass to drush command e.g. \'php\' as type for \'watchdog delete\' and  \'watchdog show\'. Optional.',       
    ),    
    'arguments' => array(
      'command' => 'The drush command to execute. For drush commands with blanks like \'watchdog show\' make sure to use single or double quotes. Mandatory.',             
    ),
  );
  $items['multi drupalupdate'] = array(
    'aliases' => array('mdr', 'drupalupdate'),
    'description' => 'Update your drupal installation to the current version minor release, if available.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT, // Find a valid Drupal root.
    'options' => array(
      /** @todo implement --sql-dump option for 'multi drupalupdate //'--sql-dump' => 'Calls multi_sql_dump for all sites before starting the drupalupdate.', */
      /** @todo implement --updatedb  option for 'multi drupalupdate //'--updatedb' => 'Execute updatedb for all sites after drupalupdate.', */     
    ),
    'examples' => array(
      'drush -r /path/to/drupal multi drupalupdate' => 'Update your drupal installation to the recommended release.',
    ),     
  );
  $items['multi site'] = array(
    'aliases' => array('msi', 'site'),
    'description' => 'Create a site in your drupal installation',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT, // Find a valid Drupal root.
    'examples' => array(
      'drush -r /path/to/drupal multi site example.com --create-directories=dir1,dir2,dir3' => 
      "Create the site 'example.com' within a settings.php,\n" . 
      "a 'files', 'themes', 'modules' directory\n" .
      "and the additional directories 'dir1', 'dir2' and 'dir3' in /path/to/drupal/sites.",
    ),
    'options' => array(
      '--create-directories' => 'Comma seperated list of aditional directories.',              
    ),
    'arguments' => array(
      'site-name'  => 'Name of the site. Mandatory.',
    ),
  );
  $items['multi create'] = array(
    'aliases' => array('mcr', 'create'),
    'description' => 'Create a multisite installation',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
    'examples' => array(
      'drush multi create /var/www 6' =>
      'Create a drupal 6 installation in /var/www with creating the directories '. 
      '6.x_sites, 6.x_backup in /var/www ' . 
      'linked as sites and backup withing the drupal installation folder.',      
    ),
    'options' => array(
      /** @todo implement --document-root option for 'multi create' //'--document-root' => "Specify a custom name for DOCUMENT_ROOT. \nDefault \${major_version}.x,\neg. 6.x for a drupal 6 release." */
      /** @todo implement --link-dirs option for 'multi create', see drush_multi_multi_site() // --create-directories option //'--link-dirs'			=> 'Comma seperated list of \'directory=linkame\',' . 'where \'directory\' is created in the parent directory of the installation ' .   //'and linked as \'linkname\' into the installation.', */      
    ),            
    'arguments' => array(
      '/path/to/installation'   => 'Directoy where drupal should be installed. Mandatory',
      'core' => 'Drupal core compatiblity. Mandatory.', 
    ), 
  );
  $items['multi sql dump'] = array(
    'aliases' => array('msq'),
    'description' => 'Exports the Drupal DB as SQL for all sites. File pattern is ${uri} and a \'_date(\'Y-m-d\TH:i\').sql suffix.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT, // Find a valid Drupal root.
    'examples' => array(
      'drush -r /path/to/drupal multi sql dump --bzip2 --comment=before-update-to-6.15 --destination=$HOME/db_backups' => 
        'Run \'sql dump\' on all sites with comment \'before-update-to-6.15\' as comment for each result file. ' .
        'The result files are bzip compressed and stored in \'db_backups\' in your home dirctory.' .
        "\nDump name schema:\n'\${uri}'_before-update-to-6.1_{date('Y-m-d\TH:i')}.sql.bz2'."
    ),
    'options' => array(
      '--destination' => 'Absolute Path to the directory where to store the sql dumps. Default /tmp. Optional.',
      '--comment'     => 'Comment for filename. May contain alphanumics, \'-\' and \'_\'. Optional.',
      '--bzip2' => 'Use bzip2 for compression of each sql dump. Optional',
      /** @todo implement --update option for sql_dump, //'--update' => 'Disable all optional modules (core/contrib), put your sites in maintainance mode and a activate a default theme. Optional', */          
      /** @todo implement --maintance option for sql_dump, //'--maintanace' => 'Put your sites in maintainance mode. Optional', */
    ),    
  );  
  return $items;
}
/** @} */ /** End of defgroup drush_hooks */

/** @todo rename all appearance of $major_version to $core */
 
/** @todo implement all rollback hooks with making use of drush_cmp_error('${ERROR_CODE}') function to identify the steps for a rollback */
  
/** @todo rename _multi_check_drupalmajorversion() to _multi_check_project() */
  
/** @todo implement "multi maintanance" with state online/offline argument */

/** @todo implement multi backup, which create an tar.gz archive of the whole installation, sites and db-dumps. */

/** @todo implement create vhost file option for 'multi site' */

/** @todo add --include-sites option for every batch command */
  
/** @todo add --exclude-sites option for every batch command */
 
/** @todo implement the possibility of using cvs as package handler for 'multi drupalupdate' and 'multi create' */
    
/** @todo implement drush_multi_multi_drupaldowngrade (continue writing...) */

/**
 * @defgroup commands
 * Commands as defined in multi_drush_command()
 * @{  
 */

/**
 * @defgroup callbacks
 * Command callbacks, see multi_drush_command() for definitions
 * @{
 */

/**
 * Commannd callback for @ref multi_status_help
 */
function multi_status() {
  
  // Prepend additional information to drush's core status
  $status = drush_core_status();	
  
  // get sites in that installation
  $sites = array();
  $sites = _multi_scan_sites(); 
  
  // Site counter
  $status .= sprintf("  %-18s: %s\n", 'Site Counter', count($sites));
   
  // Sites
  if(count($sites) != 0 ){
    $status .= sprintf("  %-18s: %s\n", 'Sites', implode(' ', $sites));   
  }
  
  // Previous local installations
  if (count($versions = _multi_previous_installations()) != 0) {
    $status .= sprintf("  %-18s: %s\n", 'Prev.Installations', implode(' ', $versions)); 
  }
  
  // Drupal core status / Available updates for drupal core
  if ($recommended_version = _multi_recommended_version(
        drush_drupal_major_version(), 'drupal', TRUE)
  ) {  
    if (drush_drupal_version() < $recommended_version) {
      $status .= sprintf("  %-18s: \033[31m\033[1m%s\033[0m\n", 
        'Drupal Core', $recommended_version . " available"
      );
    } 
    else {
      $status .= sprintf("  %-18s: %s\n", 'Drupal Core', 'Uptodate');      
    }   
  }
  else {
    $status .= sprintf("  %-18s: \033[31m\033[1m%s\033[0m\n", 
        'Drupal core status', $recommended_version . 'Unknown'
    );
  }  
  drush_print($status);   
}


/**
 * Command callback for @ref multi_nagios_help
 * @return MULTI_NAGIOS_CRITICAL when a new minor realease is available 
 */
function multi_nagios() {
  
  if (!$file = drush_get_option('file')) { 
    $file = FALSE; 
  }
  if ($recommended_version = _multi_recommended_version(drush_drupal_major_version(), 'drupal', FALSE, $file)) {
    if ((float) drush_drupal_version() < (float) $recommended_version['version']) {
      drush_print(  
        dt('DRUPAL !major.x CRITICAL: !version @ !drupal_root - drupal-!remcommended_version (!release-date) available, see !release_link for details.',
          array(
          	'!major' => drush_drupal_major_version(), 
          	'!version' => _multi_current_version(), 
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
      	dt('DRUPAL !major.x OK: !version @ !drupal_root is uptodate.',
          array(
      		'!major' => drush_drupal_major_version(),'!version' => _multi_current_version(), 
         	'!drupal_root' => drush_get_context('DRUSH_DRUPAL_ROOT'),         	
          )
        )
      );           
    }   
  }
  else {
    return $recommended_version; // FALSE if _multi_recommended_version() sets an error
  }
}

/**
 * Commannd callback for "multi moo"
 * 
 * Congratulation, you have found an easteregg
 */
function multi_moo() {
  $cowsay = " ------------\n";
  $cowsay .= "< i <3 drush >\n";
  $cowsay .= " ------------\n";
  $cowsay .= "        \   ^__^\n";
  $cowsay .= "         \  (oo)\_______\n";
  $cowsay .= "            (__)\       )\/\\\n";
  $cowsay .= "                ||----w |\n";
  $cowsay .= "                ||     ||\n";
  $cowsay .= "This drush has super cow powers!";    
  drush_print($cowsay);
}
/** @} */ /** End of defgroup callbacks */

/**
 * @defgroup multi_sql_dump 'multi sql dump'
 * Hooks for the invocation of @ref multi_sql_dump_help, see multi_drush_command() for definitions
 * @{  
 */

/**
 * Implementation of hook_drush_mycommand_validate 
 */
function drush_multi_multi_sql_dump_validate() {
  
  $destination = drush_get_option('destination');
    
  if (!empty($destination)) {
    if (!is_dir($destination)) {
      return drush_set_error('DESTINATION_IS_NOT_A_DIRECTORY', 'Supplied argument is not a directory. Aborting.');
    }
    if (!is_writable($destination)) {
      return drush_set_error('DESTINATION_NOT_WRITABLE',
        dt('Directory !dir is not writable. Aborting.', array('!dir' => $destination))
      );
    }    
  }
  else {
    drush_set_option('destination', '/tmp');
  }
   
  $comment = drush_get_option('comment');
  
  if (isset($comment) && !preg_match("/^[a-zA-Z0-9\-_]{1,}$/", $comment)) {
    return drush_set_error('MULTI_INVALID_COMMENT_ARGUMENT',
      dt('Invalid argument for comment. Aborting.')
    );
  }
  if (in_array('--bzip2', $argv = $GLOBALS['argv'])) {
    drush_set_option('--bzip2', TRUE);
  }   
}

/**
 * Implemetation of hook_drush_pre_mycommand()
 */
function drush_multi_pre_multi_sql_dump() {
  
  if(!is_writable(drush_get_option('destination'))) {
    drush_set_error('MULTI_DESTINATION_NOT_WRITABLE'); 
  }
}

/**
 * Implemetation of hook_drush_command()
 */
function drush_multi_multi_sql_dump() { 
    
  $destination = drush_get_option('destination');
  
  $sites = array();
  $sites = _multi_scan_sites();
  
  foreach ($sites as $site) {
    drush_set_option('uri', $site);
    $db_spec = _drush_sql_get_spec_from_settings(
      rtrim(drush_get_context('DRUSH_DRUPAL_ROOT'),'/') . '/sites/' . $site . '/settings.php', ''
    );
        
    // Result-file prefix
    $prefix = '_' . date('Y-m-d\TH:i') . '.sql';
    
    // Building result-file name
    $comment = drush_get_option('comment');
    if (isset($comment)) {
      $result_file = $destination . '/' . $site . '_' . $comment . $prefix;
    } 
    else {
      $result_file = $destination . '/' . $site .  $prefix;
    }
    // for --update
    $required_core_modules = array("'block'","'filter'","'node'","'system'","'user'","'watchdog'");
    $enabled_optional_modules_sql = "SELECT name FROM {system} WHERE status = 1 AND name not in (%s)";
    $disable_optional_modules_sql = "";
    $enable_optional_modules_sql = "";
    
    drush_set_option('result-file', $result_file);
    $exec = drush_sql_dump($db_spec);        
    if (drush_op('system', $exec) == 0) {
      drush_log(dt('SQL dump for !site saved in !file.', 
        array('!site' => $site, '!file' => $result_file)),
        'success'        
      );      
      if (drush_get_option('--bzip2')) {        
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
/** @} */ /** 	End of defgroup multi_sql_dump */

/**
 * @defgroup multi_exec 'multi_exec'
 * Hooks for invocation of @ref multi_exec_help, see multi_drush_command() for definition
 * @{
 */

/**
 * Implementation of hook_drush_mycommand_validate 
 * @param $command
 * @return unknown_type
 */
function drush_multi_multi_exec_validate ($command) {
  
  if (empty($command)) {
    return drush_set_error('MULTI_DRUSH_EXEC_MISSING_DRUSH_COMMAND',
      dt('Missing drush-command. Aborting,')
    );
  }
  
  // available commands and aliases
  $commands = array(
    'cache clear', 'cc',
    'refresh', 'rf',
    'update', 'up',
    'updatedb', 'updb',
    'updatecode', 'upc',
    'download', 'dl',
    'enable', 'en',
    'disable', 'dis',
    'statusmodules',  'sm',
    'watchdog show', 'ws',
    'sql query', 'sqlq',
    'watchdog delete', 'wd',    
    'cron', 'eval', 'script', 'uninstall',
  );
  
  if (!in_array($command, $commands)) {
    return drush_set_error('MULTI_DRUSH_EXEC_UNKNOWN_DRUSH_COMMAND',
      dt('Unknown drush-command. Aborting,')
    );
  }  
}

/**
 * Implemetation of hook_drush_command()
 * @param $command 
 */
function drush_multi_multi_exec($command) {  
  
  // drush's executable with absolute path
  $drush = DRUSH_COMMAND; // defined in drush.php  
  
  // --root option
  $root = ' -r ' . drush_get_context('DRUSH_DRUPAL_ROOT');
  
  // --uri option
  $uri = ' -l ';

  // prepend blank to command
  $command = ' ' . $command;
  
  // --yes option 
  // because drush_shell_exec() allows no interaction anymore
  $yes = ' -y ';  
  
  // --option, Option to pass to drush command
  $opt = ' ' . drush_get_option('option');  
  
  // --argument, Argument to pass to drush command
  $arg = ' ' . drush_get_option('argument');  
  
  // prepend php to exec
  $exec = "php ";
  
  foreach (_multi_scan_sites() as $site) {
    $exec .= $drush . $yes . $root . $uri . $site . $command . $opt . $arg;
    drush_shell_exec($exec);
    foreach (drush_shell_exec_output() as $line) {
      drush_print($line);
    }   
  }  
}
/** @} */ /** End of defgroup multi_exec */


/**
 * @defgroup multi_create 'multi_create'
 * Hooks for invocation of @ref multi_create_help, see multi_drush_command() for definition
 * @{  
 */

/**
 * @param $destination
 * @param $core
 * @return unknown_type
 */
function drush_multi_multi_create_validate($destination, $core) {
  
  if (!$destination) {
    return drush_set_error('MULTI_MISSING_DESTINATION', 
      dt('Missing argument for destination. Aborting'));
  }
  else if (!is_dir($destination)) {
    return drush_set_error('MULTI_INVALID_DESTINATION', 
      dt('Invalid argument for destination, !destination is no directory. Aborting',
        array('!destination' => $destination)
      )
    );
  }
  if (!$core) {
    return drush_set_error('MULTI_MISSING_MAJORVERSION', 
      dt('Missing argument for major-version. Aborting'));
  }
  else if (!_multi_check_drupalmajorversion($core)) {
    return drush_set_error('MULTI_INVALID_MAJORVERSION', dt('Invalid argument for major-version. Aborting'));
  }   
}


function drush_multi_multi_create($destination, $core) {
  
  // Download and  unpack drupal via drush dl to $destination / pwd
  if (!$recommended_version = _multi_recommended_version($core)) {
    return $recommended_version;
  }
  $recommended_version = 'drupal-' . strtolower($recommended_version['version']);
  
  // chdir to destination
  if (!drush_op('chdir', $destination)) {
    return drush_set_error('COULD_NOT_CHANGE_TO_DESTINATION', 
      dt('Could not change to !destination', array('!destintion' => $destination))
    );
  }
  
  // Download and  unpack drupal to $destination
  drush_set_option('package-handler', 'wget');
  drush_set_option('destination', $destination);  
  drush_invoke('download', $recommended_version);
    
  // create backup directory  
  if (!drush_op('mkdir', $core . '.x_backup')) {
    return drush_set_error('', dt(''));
  }
  
  // Create sites directory
  if (!drush_op('mkdir', $core . '.x_sites')) {
    return drush_set_error('', dt(''));
  }
  
  $dir = realpath($destination . '/' . $recommended_version . '/sites');
  
  // Copy sites/default to ${major_version}_sites
  $exec = 'cp -r ' . $dir . '/default ' . $core . '.x_sites';
  if (!drush_shell_exec($exec)) {
    return drush_set_error('MULTI_COULD_NOT_COPY_SITES_DEFAULT_DIR', 
  	  dt('Could not copy !dir to !target',
  	    array('!dir' => $dir . '/default', '!target' => $core . '.x_sites')
  	  )
  	);
  } 
  
  // Rename original sites-directory from new drupal-root to sites.bak
  // for a php rename, the directory must be empty
  $exec =   'mv ' . $dir . ' ' . $dir . '.bak';
  if (!drush_shell_exec($exec)) {
    return drush_set_error('MULTI_COULD_NOT_RENAME_SITES_DIR', 
  	  dt('Could not rename sites directory !dir to !target',
  	    array('!dir' => $dir, '!target' => $dir . '.bak')
  	  )
  	);
  }  
  // Change into new drupal directory
  if(drush_op('chdir', realpath($destination . '/' . $recommended_version))) {        
    // 4) create relative symlink for sites-directory into new drupal-root
  	$sites_dir = _multi_relative_path($destination . '/' . $core .'.x_sites');
  	if (!drush_op('symlink', $sites_dir, 'sites')) {
      return drush_set_error('DRUSH_MULTI_COULD_NOT_CREATE_SITES_SYMLINK', 
        dt('symlink(): Unable to create symlink from !source into !target for sites-directory.',
          array('!source' => $sites_dir, '!target' => getcwd() .'/sites')
        )
      );                
    }
    // Create relative symlink for backup-directory into new drupal-root
    $backup_dir = _multi_relative_path($destination . '/' . $core .'.x_backup');
    if (!drush_op('symlink', $backup_dir, 'backup')) {
      return drush_set_error('DRUSH_SM_COULD_NOT_CREATE_BACKUP_SYMLINK', 
        dt('symlink(): Unable to create relative symlink from !target into !target for backup-directory.',
          array('!target' => $sites_dir, '!target' => getcwd())          
        )
      );      
    }
  }
  else {
  	return drush_set_error('DRUSH_SM_COULD_NOT_CREATE_SITES_SYMLINK', 
      dt('symlink(): Unable to create relative symlink from !target into !target for backup-directory.',
        array('!source' => $sites_dir, '!target' => getcwd())          
      )
    );
  }
  // change to $destintion again
  drush_op('chdir', $destination);
  
  // create new symlink for document root/symlink from new drupal to ${MAJOR_VERSION}.x
  if (!drush_op('symlink', $recommended_version, $core . '.x')) {
    return drush_set_error('DRUSH_SM_COULD_NOT_SYMLINK_DOKUMENT_ROOT',
   	  dt('symlink(): Could not create symlink from !drupal to !major.x for DOCUMENT_ROOT',
   	  	array('!drupal' => $recommended_version, '!major' => $core)
   	  )
    );	
  }           	
}

function drush_multi_multi_create_rollback() {

}
/** @} */ /** End of defgroup multi_create */


/**
 * @defgroup multi_drupalupdate 'multi_drupalupdate'
 * Hooks for invocation of @ref multi_drupalupdate_help, see multi_drush_command() for definition
 * @{  
 */

function drush_multi_pre_multi_drupalupdate () {
  if (!is_writable(realpath(rtrim(drush_get_context('DRUSH_DRUPAL_ROOT'),'/') . '/..'))) {
    return drush_set_error('MULTI_DRUPALUPDATE_DESTINATION_NOT_WRITABLE',
      dt('\'!destination\' is not writable, check permission. Aborting.',
        array('!destination' => $dest)
      )
    );  
  }
}

function drush_multi_multi_drupalupdate() {
  
  // needed variables: 
  $drupal_root 		= rtrim(drush_get_context('DRUSH_DRUPAL_ROOT'),'/');
  $major_version	= drush_drupal_major_version();
  $version 			= drush_drupal_version();  
  $dest				= realpath($drupal_root . '/../');  
  $link_name        = 0;
  
  /** @todo Variable symlink name for DOCUMENT_ROOT, need'nt be 6.x when using drupal-6 */ 
  if (is_link($drupal_root)) { 
    $path_pieces = explode('/', $drupal_root);
    $link_name = $path_pieces[count($path_pieces)-1];
    drush_print($link_name);
  }
  return;
      
  // get release informtion from drupal.org via xml-xpath  
  if (!$recommended_version = _multi_recommended_version($major_version)) {
    return $recommended_version;
  } 
  $recommended_version = 'drupal-' . strtolower($recommended_version['version']);
  
  if (_multi_current_version() != $recommended_version) {  
    // 1) download and  2) unpack drupal via drush dl to parent of $drupal_root
    drush_set_option('destination', $dest);
    drush_set_option('package-handler', 'wget');
    drush_invoke('download', $recommended_version);
  }
  else {
    drush_print('Your drupal version is uptodate. Aborting');
    return MULTI_NOTHING_TO_DO; 
  }  
  // 3) Create all symlinks found in the root of the old installation for the new installation.
  //    this also include steps 3) backup sites, 4) symlink sites and 5) symlink backup, if found.
  // change into new drupal directory  
  if (drush_op('chdir', realpath($dest . '/' . $recommended_version))){
    /** @todo handle recursive link search */
    foreach (_multi_scan_links($drupal_root, FALSE) as $link) {        
      
      // if linkname exist as file or directory, rename it 
      if (is_file($link->basename) || is_dir($link->basename)) {      
        if (!drush_op('rename', $link->basename, $link->basename . '-' . date('Y-m-d\TH:i') . '-drupalupdate')) {
          drush_set_error('MULTI_COULD_NOT_RENAME',
            dt('Could not rename !existing to !backup', 
            array(
              	'!existing' => $link->basename, 
              	'!backup' =>  $link->basename . '-' . date('Y-m-d\TH:i') . '-drupalupdate'
              )
            )
          );   
        }     
      }
      // create relative symlinks to $link->target
      $filename = _multi_relative_path(realpath($link->target));
      if (!drush_op('symlink', rtrim($filename, '/'), $link->basename)) {
        drush_set_error('MULTI_COULD_NOT_CREATE_SYMLINK',
          dt('Could not create symbolic link from !target into !pwd.',
            array('!target' => rtrim($filename, '/'), '!cwd' => getcwd())
          )
        );  
      }         
    }
  }     
  if (empty($link_name)) {
    // chdir() to parent of drupal-root  
    if (drush_op('chdir', '..')) {    
      // 6) unlink document root symlink, here ${MAJOR_VERSION}.x
      if (!drush_op('unlink', $link_name)) {
        return drush_set_error('DRUSH_SM_COULD_NOT_UNLINK_DOKUMENT_ROOT',
     	  dt('unlink(): Could not unlink !doc_root.', array('!doc_root' => $link_name))
     	);
      }              
      // 7) create new symlink for document root/symlink from new drupal to ${MAJOR_VERSION}.x
      if (!drush_op('symlink', $recommended_version, $link_name)) {
        return drush_set_error('DRUSH_SM_COULD_NOT_SYMLINK_DOKUMENT_ROOT',
          dt('symlink(): Could not create symlink from !drupal to !link for DOCUMENT_ROOT.',
            array('!drupal' => $recommended_version, '!link' => $link_name)
          )
        );	
      }    
      // couldn't chdir() to parent of drupal-root
      else { 
        return drush_set_error('DRUSH_SM_COULD_CHANGE_INTO_DRUPAL_PARENT', 
          dt('chdir(): Could not change to parent of drupal directory.')
        ); 	
      }
    }
  }
  drush_log(dt('Drupalupdate finished, now you should run update.php for each site'), 'completed');
}

function drush_multi_multi_drupalupdate_rollback() {
  
}

/** @} */ /** End of defgroup multi_drupalupdate */
 
/**
 * @defgroup multi_site 'multi site'
 * Hooks for invocation of @ref multi_site_help	, see multi_drush_command() for definition
 * @{
 */

/**
 * @see multi_drush_command()
 * @param $site_name
 * @return unknown_type
 */
function drush_multi_multi_site_validate($site_name) {

  if (!$site_name) {
    return drush_set_error('MULTI_SITE_SITE_NOT_SPECIFIED', 
      dt('No site specified.')
    );
  }
  else {
    /** @todo check sitename argument with regex for valid folder name */
  }
   
  if($directories_option = drush_get_option('create-directories')) {
    $directories = array();    
    $directories = explode(',', $directories_option);
    foreach ($directories as $directory) {
      /** @todo check if the files, themes and modules folder exists */
      /** @todo check each directory name with regex, check path -> must be in the same directory */
    }
  }  
}

function drush_multi_pre_multi_site($site_name) {
  
  $sites_dir = rtrim(drush_get_context('DRUSH_DRUPAL_ROOT'),'/') . '/sites/';
  if (!is_writable($sites_dir)) {
    drush_set_error('MULTI_SITE_SITES_DIR_NOT_WRITABLE',
      dt('Site directory \'!sites\' is not writable.', array('!sites_dir' => $sites_dir))
    );
  }  
}  

function drush_multi_multi_site($site_name) {
  
  $sites_dir = rtrim(drush_get_context('DRUSH_DRUPAL_ROOT'),'/') . '/sites/';
  $site = $sites_dir . $site_name;
  
  // create $site_dir
  if (drush_op('mkdir', $site)) {
    drush_log(dt('Created site directory !dir.', array('!dir' => $site)), 'success');
    
  } 
  else {
    return drush_set_error('MULTI_SITE_COULD_NOT_CREATE_SITE_DIR',
      dt('Could not create site directory !dir.', array('!dir' => $site))
    );
  }  
  
  // change into $site_dir
  if (drush_op('chdir', $site)) {
    drush_log(dt('Changed into site directory !dir',array('!dir' => $site)), 'success');
  }
  else {
    return drush_set_error('MULTI_SITE_COULD_NOT_CWD',
       dt('Could not change to !dir.', array('!dir' => $site))
    );
  } 
        
  // copy default.settings.php to settings.php
  $settings_src = $sites_dir . 'default/default.settings.php';
  if (drush_op('copy', $settings_src, 'settings.php')) { 
    drush_log(
      dt('Copied !src to !target.', 
        array('!src' => $settings_src, '!target' => $site . '/settings.php')
      ), 'success'
    );
  }
  else {
    return drush_set_error('MULTI_SITE_COULD_NOT_COPY_DEFAULT_SETTINGS',
      dt('Could not copy !src to settings.php.', array('!src' => $settings_src))      
    );
  }

  // get additional directories from create-directories option
  if($directories_option = drush_get_option('create-directories')) {
    $directories = array();    
    $directories = explode(',', $directories_option);        
  } 
  
  // add standard directories  
  $directories[] = 'files';
  $directories[] = 'modules';  
  $directories[] = 'themes';   
  
  // create directories
  foreach ($directories as $directory) {
    if (!drush_op('mkdir', $directory)) {
      drush_set_error('MULTI_SITE_COULD_NOT_DIRECTORY',
        dt('Could not create directory \'!dir\'', array('!dir' => $directory))
      );
    }      
  }        
}
/** @} */ /** End of defgroup "multi site" */

/** @} */ /** End of defgroup commands */