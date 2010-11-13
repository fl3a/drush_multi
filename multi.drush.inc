<?php
// $Id$

include_once 'includes/multi.inc';

/**
 * @file
 * The multi implementation.   
 */

/**
 * @defgroup drush_hooks 'Drush hooks'
 * Implementation of general drush hooks
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
    case 'drush:multi-status':
      return dt("Status of your multisite installation.");    
    case 'drush:multi-exec':
      return dt('Execute a drush command for all sites in your multisite installation.');
    case 'drush:multi-drupalupdate':
      return dt('Run a drupal minor update on your multisite installation.');
    case 'drush:multi-site':
      return dt('Create a new site in your your multisite installation within a files, modules and themes directory.');
    case 'drush:multi-create':
      return dt('Create a multisite installation.');
    case 'drush:multi-sql-dump':
      return dt('Improved sql-dump.');
    case 'drush:multi-nagios':
      return 'For the use as nagios plugin. Print a message and returns with exit status.';      
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

  $items['multi-status'] = array(
    'callback' => 'multi_status',
    'aliases' => array('mst'),
    'description' => 'Status of your drupal multisite installation',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT, // Find a valid Drupal root.
    'examples' => array(
      'drush -r /path/to/drupal multi-status' => 'Displays the status.',
    ),      	
  );
  $items['multi-nagios'] = array(
    'callback' => 'multi_nagios',
    'aliases' => array('mna', 'nagios'), 
    'description' => 'Message and exit code for nagios.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'examples' => array(
      'drush -r /path/to/drupal multi-nagios' =>
      "We assume that drupal-6.14 is the recommended release.\n" . 
      "With an older version, eg. drupal-6.13 we get:\n" .
      "DRUPAL ROOT CRITICAL: drupal-6.13 @ /var/www/drupal-6.13\n" .
      " - drupal-6.14 (2009-09-16 21:40) available, see http://drupal.org/node/579476 for details.\n" .
      "with exit status 2\n" .
      "Or if we are running the recommended version:\n" .
      "'DRUPAL ROOT OK: drupal-6.14 @ /path/to/drupal/6.x is uptodate.'\n" .
      "with exit status 0.",
      'drush -r /path/to/drupal multi-nagios --file=/path/to/file' => 'Same as above with a specified file.',
      'drush -r /path/to/drupal -l example.com multi-nagios' => 
      "If there are no updates available, this will produce to following output:\n" .
      "DRUPAL SITE OK - example.com @ /path/to/drupal/example.com: All modules are uptodate.\n" .
      "If there are available updates, it might produce the following output:\n" .
      "DRUPAL SITE CRITICAL - example.com @ /path/to/drupal/sites/example.com:\n" .
      "there are updates for admin_menu,drupal,image, see http://example.com/?q=admin/reports/updates for datails.\n" .
      "NOTE: If there is a site at sites/default, this command runs on site basis, although ou have only specified the Drupal installation."
     ), 
    'options' => array(
      '--file' => "Path to release information file, this could be a (temp) file  to avoid nrpe-socket timeouts.\n" .
      "It is also possible to supply path via URI, like http://updates.drupal.org/release-history/drupal/6.x.\n" .
      "NOTE: This option is only available for checking the Drupal installation."
    ),    
  );
  $items['multi-moo'] = array(
    'aliases' => array('moo'),
    'callback' => 'multi_moo',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );
  $items['multi-exec'] = array(
    'aliases' => array('mex'),
    'description' => 'Run a drush command on all sites in your multisite installation. Implemented drush commands are: \'cache clear\', \'cron\', \'update\', \'updatecode\', \'updatedb\', \'watchdog delete\', \'watchdog show\' and \'refresh\'.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT, // Find a valid Drupal root.
    'examples' => array(
      'drush -r /path/to/drupal multi-exec cron' => 'Run all cron hooks on all sites in your multisite installation.',
      'drush -r /path/to/drupal multi-exec variable-set --argument=\'site_offline 1\' --option=\'--nocolor\''  => 'Put all sites in maintenance mode without using color highlighting on log messages.',
      'drush -r /path/to/drupal multi-exec updatedb'  => 'Execute the update.php process for all sites.',
    ),     
    'options' => array(      
      '--argument'		=> 'Argument to pass to drush command e.g. \'php\' as type for \'watchdog delete\' and  \'watchdog show\'.',
      '--option'     => 'Option to pass to drush command. or drush itself e.g. \--nocolor\' to suppress color highlighting on log messages.',       
    ),    
    'arguments' => array(
      'command' => 'The drush command to execute. For drush commands with blanks like \'watchdog show\' make sure to use single or double quotes. Mandatory.',             
    ),
  );
  $items['multi-drupalupdate'] = array(
    'aliases' => array('mdr', 'drupalupdate'),
    'description' => 'Update your drupal installation to the current version minor release, if available.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT, // Find a valid Drupal root.
    'options' => array(
      '--sql-dump' => 'Calls multi_sql_dump for all sites before starting the drupalupdate. See "drush help multi sql dump" for options and details.', 
      '--updatedb' => 'Put sites in maintenance mode before drupalupdate, execute updatedb and put sites back from maintenance mode after drupalupdate.',      
    ),
    'examples' => array(
      'drush -r /path/to/drupal multi-drupalupdate' => 'Update your drupal installation to the recommended release.',
      'drush -r /path/to/drupal multi-drupalupdate --sql-dump --comment="before drupaldate" --update' =>
      'Runs a "multi-drupalupdate" on /path/to/drupal with making sql dumps of all sites with optional comment "before drupalupdate" for sql dump filenames and execution of "updatedb" afterwards',   
    ),     
  );
  $items['multi-site'] = array(
    'aliases' => array('msi', 'site'),
    'description' => 'Create a site in your drupal installation',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT, // Find a valid Drupal root.
    'examples' => array(
      'drush -r /path/to/drupal multi-site example.com --create-directories=dir1,dir2,dir3' => 
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
  $items['multi-create'] = array(
    'aliases' => array('mcr', 'create'),
    'description' => 'Create a multisite installation',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
    'examples' => array(
      'drush multi-create /var/www --core=6.x' =>
      'Creates a Drupal 6 multisite installation in /var/www with creating the directories '. 
      '6.x_backup, 6.x_profiles and 6.x_sites, in /var/www ' . 
      'linked as sites and backup withing the drupal installation folder.',
      'drush multi-create /var/www --makefile=/path/to/makefile.make' => 
      'Creates a Drupal multisite installation in /var/www by using the supplied makefile' .
      ' at /path/to/makefile.make.',      
    ),
    'options' => array(
      /** @todo implement --document-root option for 'multi create' */ //'--document-root' => "Specify a custom name for DOCUMENT_ROOT. \nDefault \${major_version}.x,\neg. 6.x for a drupal 6 release.",
      '--makefile' => "Path to drush_make makefile. \nThe makefile must contain the core specification (eg. \"core = 6.x\" for Drupal 6)\nand Drupal as project (projects[] =  drupal).",
      '--core'     => "Drupal core version (5.x, 6.x). \nTakes default from pm_parse_project_version(array('drupal') if not supplied.",     
    ),            
    'arguments' => array(
      '/path/to/installation'   => 'Path to where drupal should be installed. Mandatory',
      /** @todo Continue writung 'DocumentRoot' for multi-create */ //'DocumentRoot' =>  'Name of DocumentRoot, name of directory that should be created within "/path/to/installation". Optional, default ${core}.x.',
    ), 
  );
  $items['multi-sql-dump'] = array(
    'aliases' => array('msq'),
    'description' => 'Exports the Drupal DB as SQL. File pattern is ${uri} and a \'_date(\'Y-m-d\TH:i\').sql suffix.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_DATABASE, // Bootstrap database. 
    'examples' => array(
      'drush -r /path/to/drupal -l example.com multi-sql-dump --bzip2 --comment=before-update-to-6.15 --target=$HOME/db_backups' => 
        'Run \'sql dump\' on example.com with \'before-update-to-6.15\' as comment. ' . "\n" .
        'The result file will be bzip compressed and stored in \'db_backups\' in your home directory.' .
        "\nDump name schema:\n\${uri} [ _ \${comment} _ \${date('Y-m-d\TH:i')] .sql [.bz2], \nwhere \${url} is example.com supplied by -l/--uri."
    ),
    'options' => array(
      '--target' => 'Absolute Path to the directory where to store the sql dumps. Default /tmp.',
      '--dump-type' => "Arguments:\n" .
      "\"full\" Creates one file for all tables, default.\n" .
      '"tables" Creates for each table within each database a seperate file.',
      '--comment'     => 'Comment for filename. May contain alphanumics, \'-\' and \'_\'.',
      '--datetime' => 'Adds an optional datetime string to each result file. Format YYYY-MM-DDThh:mm',
      '--bzip2' => 'Use bzip2 for compression of each sql dump.',
    ),    
  );  
  return $items;
}
/** @} */ /** End of defgroup drush_hooks */
 /**
 * @defgroup callbacks
 * Command callbacks, see multi_drush_command() for definitions
 * @{
 */

/**
 * Command callback for @ref multi_status
 */
function multi_status() {

  /** @todo Implement --pipe support */
  // Prepend additional information to drush's core status
  drush_core_status(); 
  
  $table = $sites = array();
  
  // get sites in that installation
  $sites = _multi_scan_sites(); 
  
  // Site counter
  $table['Site Counter'] = count($sites);
   
  // Sites
  if(count($sites) != 0 ){
    $table['Sites'] = implode(' ', $sites);
  }
  
  // Previous local installations
  if ($versions = _multi_previous_installations() != 0) {
    $table['Prev. Installations'] = implode(' ', $versions); 
  }
  
  // Drupal core status / Available updates for drupal core
  if ($recommended_version = _multi_recommended_version(
    drush_drupal_major_version().'.x', 'drupal', TRUE)
  ) {  
    if (drush_drupal_version() < $recommended_version) {
      $table['Drupal core status'] = $recommended_version . " available";
    } 
    else {
      $table['Drupal core status'] = 'Uptodate';      
    }   
  }
  else {
     $table['Drupal core status'] = 'Unknown';
  }  
  drush_print_table(drush_key_value_to_array_table($table), TRUE);
}

/**
 * Command callback for @ref multi_nagios_help
 * 
 * Calls _multi_nagios_site() if a valid site was supplied or _multi_nagios_root()
 * 
 * @return MULTI_NAGIOS_CRITICAL (2) or MULTI_NAGIOS_OK (0) 
 */
function multi_nagios() {
 
  // get -l/--uri option
  (drush_get_option('uri')) ? $uri = drush_get_option('uri') : $uri = drush_get_option('l');  
    
  if (!empty($uri)) {
    if (_multi_check_site($uri)) {
      // multi nagios was called with -l/--uri
      return _multi_nagios_site();
    }
  }
  else {
    // multi nagios was called with -r/--root only
    return _multi_nagios_root();
  }
}

/**
 * Command callback for "multi moo"
 * 
 * Congratulation, you have found an easteregg
 */
function multi_moo() {
  
  $cowsay  = "\n ------------\n";
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
 * @defgroup commands
 * Commands as defined in multi_drush_command()
 * @{  
 */


/**
 * @defgroup multi_sql_dump multi-sql-dump
 * Hooks for the invocation of @ref multi_sql_dump_help, see multi_drush_command() for definitions
 * @{  
 */

/**
 * Implementation of drush_hook_COMMAND_validate 
 */
function drush_multi_sql_dump_validate() {

  // Check --bzip2 option
  if (in_array('--bzip2', $GLOBALS['argv'])) {
    drush_set_option('bzip2', TRUE);
  }
   
  // Check --datetime option
 if (in_array('--datetime', $GLOBALS['argv'])) {
    drush_set_option('datetime', '_' . date('Y-m-d\TH:i')) ;
  }
  
  // check target argument
  $target = drush_get_option('target');
  if (!empty($target)) {
    if (!is_dir($target)) {
      return drush_set_error('MULTI_TARGET_IS_NOT_A_DIRECTORY', 'Supplied argument for target is not a directory. Aborting.');
    }
    
    // check target for write access 
    if (!is_writable($target)) {
      return drush_set_error('MULTI_TARGET_NOT_WRITABLE',
        dt('Directory !dir is not writable. Aborting.', array('!dir' => $target))
      );
    }    
  }
  else {
    // set /tmp as default
    drush_set_option('target', '/tmp');
  }
  
  // check --comment argument
  $comment = drush_get_option('comment');  
  if (isset($comment) && !preg_match("/^[a-zA-Z0-9\-_]{1,}$/", $comment)) {
    return drush_set_error('MULTI_INVALID_COMMENT_ARGUMENT',
      dt('Invalid argument for comment, only alphanumerics, "-" and "_" allowed. Aborting.')
    );
  }
  
  // check --dump-type argument
  $dump_type = drush_get_option('dump-type','full');
  // list of allowed dump type arguments 
  $dump_types = array('full','tables');
  if (!in_array($dump_type, $dump_types )) {
    return drush_set_error('MULTI_INVALID_DUMP_TYPE_ARGUMENT',
      dt('Invalid argument for dump-type. Aborting.')
    );
  }
}

/**
 * Implementation of drush_hook_COMMAND()
 */
function drush_multi_sql_dump() { 
   
  $dump_type = drush_get_option('dump-type', 'full');
  $db_spec = _drush_sql_get_db_spec();
    
  $site = drush_get_option(array('uri','l'));
  $datetime = drush_get_option('datetime');
  $comment  = drush_get_option('comment'); 
  $target   = drush_get_option('target');    
   
  // full: full sql dump 
  if ($dump_type == 'full') {
                    
    //  Building result-file name 
    $result_file = $target . '/' . $site . $datetime . $comment . '.sql';
    
    // set --result-file argument for drush_sql_dump()
    drush_set_option('result-file', $result_file);
    
    // preperation for --update

    /** @tode Implement --update option for disabling all non-core modules */

    /*
    $required_core_modules = array("'block'","'filter'","'node'","'system'","'user'","'watchdog'");
    $enabled_optional_modules_sql = "SELECT name FROM {system} WHERE status = 1 AND name not in (%s)";
    $disable_optional_modules_sql = "";
    $enable_optional_modules_sql = "";
    */
    
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
  // tables: seperate sql dump/file for each table 
  else {

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
      if (drush_shell_exec($exec . '>' . $result_file) == 0) {
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
          drush_print('bzip2');
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
}

/** @} */ /** 	End of defgroup multi_sql_dump */

/**
 * @defgroup multi_exec multi-exec
 * Hooks for invocation of @ref multi_exec_help, see multi_drush_command() for definition
 * @{
 */

/**
 * Implementation of drush_hook_COMMAND_validate 
 * @param $command
 */
function drush_multi_exec_validate ($command) {
  
  if (empty($command)) {
    return drush_set_error('MULTI_DRUSH_EXEC_MISSING_DRUSH_COMMAND',
      dt('Missing drush-command. Aborting,')
    );
  }
  
  // available commands and aliases
  $commands = array(
    'cache-clear', 'cc',
    'pm-refresh', 'rf',
    'pm-update', 'up',
    'pm-updatecode', 'upc',
    'pm-download', 'dl',
    'pm-enable', 'en',
    'pm-disable', 'dis',
    'pm-uninstall', 
    'statusmodules',  'sm',
    'watchdog-show', 'ws',
    'watchdog-delete', 'wd-del',
    'sql-query', 'sqlq',
    'variable-set', 'vset',
    'variable-get', 'vget',
    'variable-delete', 'vdel',
    'updatedb', 'updb',  
    'cron', 'eval', 'script', 
  );
  
  if (!in_array($command, $commands)) {
    return drush_set_error('MULTI_DRUSH_EXEC_UNKNOWN_DRUSH_COMMAND',
      dt('Unknown drush-command. Aborting,')
    );
  }  
}

/**
 * Implementation of drush_hook_COMMAND()
 * @param $command 
 */
function drush_multi_exec($command) {  
  
  // Determine a proper way to call drush again
  // defined in includes/environment.php, initialized via drush_find_drush()
  // Consists of:
  // absolute path to php, absolute path to drush's executable and appended php option, e.g:
  // /usr/bin/php /usr/local/bin/drush/drush.php --php=/usr/bin/php
  $drush = DRUSH_COMMAND; 
  
  // --root option
  $root = ' -r ' . drush_get_context('DRUSH_DRUPAL_ROOT');
  
  // --uri option
  $uri = ' -l ';

  // prepend blank to command
  $command = ' ' . $command;
  
  // --yes option 
  // because drush_shell_exec() allows no interaction anymore
  $yes = ' -y';  
  
  // --argument, Argument to pass to drush command
  $arg = ' ' . drush_get_option('argument');  
  
  // --option, Option to pass to drush command or drush itself
  $opt = ' ' . drush_get_option('option');  
  
  $sites = array();
  $sites = _multi_scan_sites();
  
  foreach ($sites as $site) {
    $cmd = $drush . $yes . $root . $uri . $site . $command . $arg . $opt;
    drush_shell_exec($cmd);
    
    foreach (drush_shell_exec_output() as $line) {
      drush_print($line);
    } 
  }  
}
/** @} */ /** End of defgroup multi_exec */

/**
 * @defgroup multi_create multi-create
 * Hooks for invocation of @ref multi_create_help, see multi_drush_command() for definition
 * @{  
 */

/**
 * Implementation of drush_hook_COMMAND_validate()
 * @param $destination
 */
function drush_multi_create_validate($destination, $document_root = NULL) {
  
  // Check destination argument
  if (!$destination) {
    return drush_set_error('MULTI_MISSING_DESTINATION', 
      dt('Missing argument for destination. Aborting'));
  }
  else {
    if (!is_dir($destination)) {
      return drush_set_error('MULTI_INVALID_DESTINATION', 
        dt('Invalid argument for destination, "!destination"  is no directory. Aborting',
          array('!destination' => $destination)
        )
      );
    }
    if (!is_writable($destination)){
      return drush_set_error('MULTI_DESTINATION_NOT_WRITABLE', 
        dt('Destinattion !destination is not writable. Aborting',
          array(            
            '!destination' => $destination,    
          )
        )
      );
    }    
  }
    
  // Check makefile argument
  $makefile = drush_get_option('makefile');
  
  if ($makefile) {
    if (!is_file($makefile)) {
      return drush_set_error('MULTI_INVALID_MAKEFILE', 
        dt('Invalid argument for makefile, file !makefile does not exist. Aborting',
          array('!makefile' => $makefile)
        )
      );
    }
    drush_set_option('makefile', realpath($makefile));
    // Check for required make command (drush_make)
    if (!drush_is_command('make')) {
      return drush_set_error('MULTI_MAKE_UNAVAILABLE', 'Could not find drush_make');    
    }
    $core = FALSE;
    $parsed_makefile = drush_make_parse_info_file($makefile);
    $core = (int) $parsed_makefile['core'];
    // core was not specified in makefile
    if (!$core) {
      return drush_set_error('MULTI_MAKE_NO_CORE', 'Core must be supplied in the makefile. Aborting.');
    }
    // Set makefile`s core
    drush_set_option('core', $core);
    if (!in_array('drupal', $parsed_makefile['projects'])){
      return drush_set_error('MULTI_MAKE_NO_DRUPAL','Drupal must be supplied in the makefile. Aborting.');      
    }
  }
  // Check core argument
  else {
    // Set recommended drupal core version 
    $core = drush_get_option('core');
    if (!$core) {
      $parsed_version = array();
      $parsed_version = pm_parse_project_version(array('drupal'));
      $recommended_core = $parsed_version['drupal']['drupal_version'];
      drush_set_option('core', $recommended_core);
    }
    // Check supplied core version
    else if (!_multi_check_project($core)) {
      return drush_set_error('MULTI_INVALID_MAJORVERSION', dt('Invalid argument for major-version. Aborting'));
    }   
  }
}

/**
 * Implementation of drush_hook_COMMAND()
 * @param $destination
 */
function drush_multi_create($destination, $document_root = NULL) {
  
  // convert $destination to an absolute path
  $destination = realpath($destination);
    
  // Use supplied makefile and build it at $destination
  if ($makefile = rtrim(drush_get_option('makefile'), '/')) {    
    if (!$recommended_version = _multi_recommended_version($core)) {
    //drush_op('system', 'drush make ' . $makefile . ' ' . $destination . '/' . $recommended_version);
      return $recommended_version;
    }
    $recommended_version = 'drupal-' . strtolower($recommended_version['version']);
    drush_invoke('make', $makefile, $destination . '/' . $recommended_version);
  }
  // Else download a naked drupal to $destination
  else {
     // Get recommended release of specified core version
    $core = drush_get_option('core');
    if (!$recommended_version = _multi_recommended_version($core)) {
      return $recommended_version;
    }
    $recommended_version = 'drupal-' . strtolower($recommended_version['version']);
      
    drush_set_option('package-handler', 'wget');
    drush_set_option('destination', $destination);  
    drush_set_context('DRUSH_NOCOLOR', 1);
    drush_invoke('pm-download', $recommended_version);
    drush_set_context('DRUSH_NOCOLOR', 0);
  }
  
  // chdir to destination
  if (!drush_op('chdir', $destination)) {
    return drush_set_error('MULTI_COULD_NOT_CHANGE_TO_DESTINATION', 
      dt('Could not change to !destination. Aborting.', 
        array('!destination' => $destination)
      )
    );
  }  

  // create backup directory  
  if (!drush_op('mkdir', $core . '_backup')) {
    return drush_set_error('MULTI_COULD_NOT_CREATE_BACKUP_DIR', 
      dt('Could not create backup directory "!pwd/!core.x_backup". Aborting.',
        array('!pwd' => drush_cwd(), '!core' => $core)
      )
    );
  }
  
  // Create sites directory
  if (!drush_op('mkdir', $core . '_sites')) {
    return drush_set_error('MULTI_COULD_NOT_CREATE_SITES_DIR', 
      dt('Could not create sites directory "!pwd/!core_sites". Aborting.',
        array('!pwd' => drush_cwd(), '!core' => $core)
      )
    );
  }
  
  $dir = realpath($destination . '/' . $recommended_version . '/sites');
  
  // Copy content of sites directory to ${major_version}_sites
  $exec = 'cp -r ' . $dir . '/* ' . $core . '_sites';
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

  // create profiles directory
  if (!drush_op('mkdir', $core . '_profiles')) {
    return drush_set_error('MULTI_COULD_NOT_CREATE_PROFILES_DIR', 
      dt('Could not create profiles directory "!pwd/!core_profiles". Aborting.',
        array('!pwd' => drush_cwd(), 
        '!core' => $core)
      )
    );
  }
  
  $dir = realpath($destination . '/' . $recommended_version . '/profiles');
  
  // Copy content of profiles directory to ${major_version}_profiles
  $exec = 'cp -r ' . $dir . '/* ' . $core . '_profiles';
  if (!drush_shell_exec($exec)) {
    return drush_set_error('MULTI_COULD_NOT_COPY_PROFILES_DEFAULT_DIR', 
  	  dt('Could not copy !dir to !target',
  	    array('!dir' => $dir . '/default', '!target' => $core . '_profiles')
  	  )
  	);
  } 
  
  // Rename original profiles-directory from new drupal-root to profiles.bak
  // for a php rename, the directory must be empty
  $exec =   'mv ' . $dir . ' ' . $dir . '.bak';
  if (!drush_shell_exec($exec)) {
    return drush_set_error('MULTI_COULD_NOT_RENAME_DEFAULT_DIR', 
  	  dt('Could not rename profiles directory !dir to !target',
  	    array('!dir' => $dir, '!target' => $dir . '.bak')
  	  )
  	);
  }
  
  // Change into new drupal directory
  if(drush_op('chdir', realpath($destination . '/' . $recommended_version))) {        
    // 4) create relative symlink for sites-directory into new drupal-root
  	$sites_dir = _multi_relative_path($destination . '/' . $core .'_sites');
  	if (!drush_op('symlink', $sites_dir, 'sites')) {
      return drush_set_error('MULTI_COULD_NOT_CREATE_SITES_SYMLINK', 
        dt('symlink(): Unable to create symlink from !source into !target for sites-directory.',
          array('!source' => $sites_dir, '!target' => getcwd() .'/sites')
        )
      );                
    }
    // Create relative symlink for backup-directory into new drupal-root
    $backup_dir = _multi_relative_path($destination . '/' . $core .'_backup');
    if (!drush_op('symlink', $backup_dir, 'backup')) {
      return drush_set_error('MULTI_COULD_NOT_CREATE_BACKUP_SYMLINK', 
        dt('symlink(): Unable to create relative symlink from !target into !target for backup-directory.',
          array('!target' => $sites_dir, '!target' => getcwd())          
        )
      );      
    }
    // Create relative symlink for profiles-directory into new drupal-root
    $profiles_dir = _multi_relative_path($destination . '/' . $core .'_profiles');
    if (!drush_op('symlink', $profiles_dir, 'profiles')) {
      return drush_set_error('MULTI_COULD_NOT_CREATE_PROFILES_SYMLINK', 
        dt('symlink(): Unable to create relative symlink from !target into !target for backup-directory.',
          array('!target' => $profiles_dir, '!target' => getcwd())          
        )
      );      
    }
  }
  else {
  	return drush_set_error('MULTI_COULD_NOT_CREATE_SITES_SYMLINK', 
      dt('symlink(): Unable to create relative symlink from !target into !target for backup-directory.',
        array('!source' => $sites_dir, '!target' => getcwd())          
      )
    );
  }
  // change to $destintion again
  drush_op('chdir', $destination);
  
  // create new symlink for document root/symlink from new drupal to ${MAJOR_VERSION}.x
  if (!drush_op('symlink', $recommended_version, $core)) {
    return drush_set_error('MULTI_COULD_NOT_SYMLINK_DOKUMENT_ROOT',
   	  dt('symlink(): Could not create symlink from !drupal to !major for DOCUMENT_ROOT',
   	  	array('!drupal' => $recommended_version, '!major' => $core)
   	  )
    );	
  }           	
}

/**
 * Implementation of drush_hook_COMMAND_rollback()
 */
function drush_multi_create_rollback($destination, $document_root = NULL) {
  /** @todo implement drush_multi_create_rollback() */
}

/** @} */ /** End of defgroup multi_create */

/**
 * @defgroup multi_drupalupdate multi-drupalupdate
 * Hooks for invocation of @ref multi_drupalupdate_help, see multi_drush_command() for definition
 * @{  
 */

/**
 * Implementation of drush_hook_COMMAND_validate()
 * 
 * Performs general checks and set needed variables via drush_set_context()
 */
function drush_multi_drupalupdate_validate () {
  
  // Check if parent directory of DRUSH_DRUPAL_ROOT is writable
  if (!is_writable(realpath(rtrim(drush_get_context('DRUSH_DRUPAL_ROOT'),'/') . '/..'))) {
    drush_set_error('MULTI_DRUPALUPDATE_DESTINATION_NOT_WRITABLE',
      dt('\'!destination\' is not writable, check permission. Aborting.',
        array('!destination' => $dest)
      ) 
    );  
  }
  // Get DRUSH_DRUPAL_ROOT without trailing '/'
  $drupal_root = rtrim(drush_get_context('DRUSH_DRUPAL_ROOT'),'/');

  // Check if DRUSH_DRUPAL_ROOT is a symbolic link
  if (is_link($drupal_root)) { 
    // Get the name of the symbolic link
    $path_pieces = explode('/', $drupal_root);
    $link_name = $path_pieces[count($path_pieces)-1];
    // Set  the name of the symbolic link via drush_set_context()
    drush_set_context('DRUPAL_ROOT_SYMLINK', $link_name);
    // Set  DRUSH_DRUPAL_ROOT without trailing '/' via drush_set_context()
    drush_set_context('DRUSH_DRUPAL_ROOT', $drupal_root);
  }
  else {
    drush_set_error('MULTI_DRUPALUPDATE_DRUPAL_ROOT_IS_NO_SYMLINK',
      dt('Drupal root !drupal_root is not a symbolic link. Aborting.',
        array('!drupal_root' => $drupal_root)
      )
    );
  }
  
  // Get release informtion from drupal.org via xml-xpath  
  if (!$recommended_version = _multi_recommended_version(drush_drupal_major_version() . '.x')) {
    // return with exit code of _multi_recommended_version() if an error ocurrs
    return $recommended_version;
  } 
  $recommended_version = 'drupal-' . strtolower($recommended_version['version']);
  // Set recommended version via drush_set_context()
  drush_set_context('DRUPAL_RECOMMENDED_VERSION', $recommended_version);
  
  // Check if we need to update by comparing current and recommended version
  if (_multi_current_version() == $recommended_version) {  
    drush_print('Your drupal version is uptodate. Aborting');
    return MULTI_NOTHING_TO_DO; 
  }
  
  // Set '--updatedb' option, if supplied to TRUE via drush_set_option() 
  if (in_array('--updatedb', $argv = $GLOBALS['argv'])) {
    drush_set_option('--updatedb', TRUE);
  }

  // Set '--sql-dump' option, if supplied to TRUE via drush_set_option() 
  if (in_array('--sql-dump', $argv = $GLOBALS['argv'])) {
    drush_set_option('--sql-dump', TRUE);
  }
}

/**
 * Implementation of drush_hook_pre_COMMAND()
 * 
 * Invokes 'multi-exec' with  'sql-dump' argument if '--sql-dump' option is set
 * 
 * Invokes 'multi-exec' with  'variable-set' and 'site_offline 1' argument if '--updatedb' option is set 
 */
function drush_multi_pre_multi_drupalupdate() {
  
  if (drush_get_option('--sql-dump')) { 
    drush_set_option('option', '--nocolor');
    drush_invoke('multi-sql-dump'); 
  } 
  if (drush_get_option('--updatedb')) {
    // put sites in maintenance mode
    drush_set_option('argument', 'site_offline 1');
    drush_set_option('option', '--nocolor');
    drush_invoke('multi-exec', 'variable-set');
    drush_unset_option('argument');
    drush_unset_option('option');   
  }
}

/**
 * Implementation of drush_hook_COMMAND()
 * 
 * @see drush_multi_drupalupdate_validate() for initialisation of needed variables via drush_set_context()
 * @see <a href="http://netzaffe.de/drupal-6-multisiteumgebung-mit-postgresql-unter-debian-4/drupal-update-in-8-schritten.html">Drupal Update in 8 Steps (Drupal Update in 8 Schritten (german))</a>
 * for the corresponding steps.  
 */
function drush_multi_drupalupdate() {
  
  // needed variables: 
  $drupal_root 		    = drush_get_context('DRUSH_DRUPAL_ROOT');
  $link_name            = drush_get_context('DRUPAL_ROOT_SYMLINK');
  $recommended_version  = drush_get_context('DRUPAL_RECOMMENDED_VERSION');
  $version 			    = drush_drupal_version();  
  $dest				    = realpath($drupal_root . '/../');    
  
  // 1) download and  2) unpack drupal via drush dl to parent of $drupal_root
  drush_set_option('destination', $dest);
  drush_set_option('package-handler', 'wget');
  drush_set_context('DRUSH_NOCOLOR', 1);
  drush_invoke('pm-download', $recommended_version);
  drush_set_context('DRUSH_NOCOLOR', 0);
    
  // 3) Create all symlinks found in the root of the old installation for the new installation.
  //    this also include steps 3) backup sites, 4) symlink sites and 5) symlink backup, if found.
  
  _multi_preserve_symlinks($drupal_root, $dest . '/' . $recommended_version);
  
  // chdir() to parent of drupal-root  
  if (drush_op('chdir', '..')) {    
    // 6) unlink document root / old Drupal version
    if (!drush_op('unlink', $link_name)) {
      return drush_set_error('DRUSH_SM_COULD_NOT_UNLINK_DOKUMENT_ROOT',
   	    dt('unlink(): Could not unlink !doc_root.', array('!doc_root' => $link_name))
   	  );
    }              
    // 7) create symbolic link for document root / new Drupal version
    if (!drush_op('symlink', $recommended_version, $link_name)) {
      return drush_set_error('DRUSH_SM_COULD_NOT_SYMLINK_DOKUMENT_ROOT',
        dt('symlink(): Could not create symlink from !drupal to !link for DOCUMENT_ROOT.',
          array('!drupal' => $recommended_version, '!link' => $link_name)
        )
      );	
    }
  }    
  // couldn't chdir() to parent of drupal-root
  else { 
    return drush_set_error('DRUSH_SM_COULD_CHANGE_INTO_DRUPAL_PARENT', 
      dt('chdir(): Could not change to parent of drupal directory.')
    ); 	
  }  
}

/**
 * Implementation of drush_hook_COMMAND_rollback()
 */
function drush_multi_drupalupdate_rollback() {
  /** @todo implement drush_multi_drupalupdate_rollback() */
}

/**
 * Implementation of drush_hook_post_COMMAND()
 * 
 * Invokes 'multi-exec' with  'variable-set' and 'site_offline 0' argument if '--updatedb' option is set
 * 
 * Invokes 'multi-exec updatedb' if '--updatedb' option is set
 */
function drush_multi_post_multi_drupalupdate() {
  
  if (drush_get_option('--updatedb')) {
    // execute updatedb
    drush_set_option('option','--nocolor');
    drush_invoke('multi-exec', 'updatedb');
    // put sites back from maintenance mode
    drush_set_option('argument', 'site_offline 0');
    drush_invoke('multi-exec', 'variable-set');  
    drush_unset_option('option');
    drush_unset_option('argument');  
    drush_log(dt('Drupalupdate finished.'), 'completed');
  }
  else {
    drush_log(dt('Drupalupdate finished, now you should run update.php for each site'), 'completed');
  }
}

/** @} */ /** End of defgroup multi_drupalupdate */
 
/**
 * @defgroup multi_site multi-site
 * Hooks for invocation of @ref multi_site_help	, see multi_drush_command() for definition
 * @{
 */

/**
 * Implementation of drush_hook_COMMAND_validate()
 * @param $site_name
 */
function drush_multi_site_validate($site_name) {

  if (!$site_name) {
    return drush_set_error('MULTI_SITE_MISSING_SITE_ARGUMENT', 
      dt('Missing site argument.')
    );
  }
  else {
    /** @todo check sitename argument with regex for valid folder name */
  }
   
  if($directories_option = drush_get_option('create-directories')) {
    $directories = array();    
    $directories = explode(',', $directories_option);
    foreach ($directories as $directory) {
      /** @todo check each directory name with regex */
      ltrim($directory, '/');
    }
  }  
}

/**
 * Implementation of drush_hook_pre_COMMAND()
 * @param $site_name
 */
function drush_multi_pre_multi_site($site_name) {
  
  $sites_dir = rtrim(drush_get_context('DRUSH_DRUPAL_ROOT'),'/') . '/sites/';
  if (!is_writable($sites_dir)) {
    return drush_set_error('MULTI_SITE_SITES_DIR_NOT_WRITABLE',
      dt('Site directory \'!sites_dir\' is not writable.', array('!sites_dir' => $sites_dir))
    );
  }
  if (is_dir($sites_dir .'/' . $site_name)) {
    return drush_set_error('MULTI_SITE_SITE_DIR_ALREADY_EXISTS',
      dt('The supplied sitename !site_name already exists within !sites_dir. Aborting.', 
        array(
        '!site_name' => $site_name, 
        '!sites_dir' => $sites_dir
        )
      )
    );
  }  
}  

/**
 * Implementation of drush_hook_COMMAND()
 * @param $site_name
 */
function drush_multi_site($site_name) {
  
  $sites_dir = rtrim(drush_get_context('DRUSH_DRUPAL_ROOT'),'/') . '/sites/';
  $site = $sites_dir . $site_name;
  
  // create $site_dir
  if (!drush_op('mkdir', $site)) {
    drush_set_error('MULTI_SITE_COULD_NOT_CREATE_SITE_DIR',
      dt('Could not create site directory !dir.', array('!dir' => $site))
    );
  }  
  
  // change into $site_dir
  if (!drush_op('chdir', $site)) {
    drush_set_error('MULTI_SITE_COULD_NOT_CWD',
       dt('Could not change to !dir.', array('!dir' => $site))
    );
  } 
  
  // create a settings.php by copying
  switch (drush_drupal_major_version()){    
    case '5':  
      // copy sites/default/settings.php to settings.php for 5.x
      $settings_src = $sites_dir . 'default/settings.php';
      if (!drush_op('copy', $settings_src, 'settings.php')) { 
        drush_set_error('MULTI_SITE_COULD_NOT_COPY_DEFAULT_SETTINGS',
          dt('Could not copy !src to settings.php.', array('!src' => $settings_src))      
        );
      }
      break;
    case '6':
    case '7':
      // copy sites/default.settings.php to settings.php for 6.x and 7.x
      $settings_src = $sites_dir . 'default/default.settings.php';
      if (!drush_op('copy', $settings_src, 'settings.php')) { 
        drush_set_error('MULTI_SITE_COULD_NOT_COPY_DEFAULT_SETTINGS',
          dt('Could not copy !src to settings.php.', array('!src' => $settings_src))      
        );
      }
      break;
  }      
  
  $directories = array();
  
  // add standard directories  
  $directories[] = 'files';
  $directories[] = 'modules';  
  $directories[] = 'themes';   
  
  // get additional directories from create-directories option
  if($directories_option = drush_get_option('create-directories')) {
    $tmp = array();    
    $tmp = explode(',', $directories_option);
    $directories = array_merge($directories, $tmp);
  } 
    
  // create directories
  foreach ($directories as $directory) { 	   
    if (!drush_op('mkdir', $directory, 0755, TRUE)) { 	 
	  drush_set_error('MULTI_SITE_COULD_NOT_DIRECTORY', 	 
	    dt('Could not create directory \'!dir\'', array('!dir' => $directory)) 	 
	  ); 	 
	} 	 
  }          
}

/**
 * Implementation of drush_hook_post_COMMAND()
 *  
 * @param $site_name
 */
function drush_multi_post_multi_site($site_name){
  
  $site_path    = drush_get_context('DRUSH_DRUPAL_ROOT') . '/sites/' . $site_name;
  
  $apache_user  = drush_get_option('multi-apache-user');
  $apache_group = drush_get_option('multi-apache-group');  
  
  if ($apache_user){
    drush_op('chown', $site_path . '/settings.php', $apache_user);
    drush_op('chown', $site_path . '/files', $apache_user);
  }
  if ($apache_group){
    drush_op('chgrp', $site_path . '/settings.php', $apache_group);
    drush_op('chgrp', $site_path . '/files', $apache_group);
  }
}

/**
 * Implementation of drush_hook_COMMAND_rollback()
 * @param $site_name
 */
function drush_multi_site_rollback($site_name) {
  if (!drush_cmp_error('MULTI_SITE_COULD_NOT_CREATE_SITE_DIR')) {
    delete_dir(rtrim(drush_get_context('DRUSH_DRUPAL_ROOT'),'/') . '/sites/'. $site_name);
  }
}
/** @} */ /** End of defgroup multi_sql_dump */
  
/** @} */ /** End of defgroup commands */