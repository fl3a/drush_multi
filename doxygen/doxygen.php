<?php 
// $Id$
 
//This file is for doxygen documenation purposes only 
  
/**
 * @file
 * Doxygen documentation 
 */
  
/** 
 * @mainpage 
 * Documentation of drush_multi - http://drupal.org/project/drush_multi
 * 
 * Drush_multi is an extension to the Drupal Shell aka drush (http://drupal.org/project/drush) which deals with Drupal multisites.
 * 
 * @subpage intro "Introduction"
 * 
 * @subpage install "Installation"
 * 
 * @subpage cmds "Commands"
 */ 

/** 
 * @page intro Introduction
 * Within this structure and this drush command, it is now possible to do that via drush.
 * Now can drush update drupal.
 * 
 * Here is the very specific structure this command deals with for @ref multi_drupalupdate_help and @ref multi_create_help :
 * @code
 * '
 * drupal/
 * |-- 6.x -> drupal-6.14
 * |-- 6.x_backup
 * |-- 6.x_profiles
 * |-- 6.x_sites
 * |   |-- all   
 * |   |-- default
 * |   |-- example.com
 * |   |   |-- files
 * |   |   |-- modules
 * |   |   `-- themes
 * |   `-- sub.example.com 
 * |       |-- files
 * |       |-- modules
 * |       `-- themes 
 * `-- drupal-6.14
 *    |-- backup -> ../6.x_backup
 *    |-- includes
 *    |-- misc
 *    |-- modules
 *    |-- profiles -> ../6.x_profiles
 *    |-- scripts
 *    `-- sites  -> ../6.x_sites
 * '
 * @endcode
 * @note Since commit #305248 (http://drupal.org/cvs?commit=305248), the linkname, here 6.x is free choosable
 * and need not follow the schema ${core}.x anymore. 
 */

/** 
 * @page install Installation
 * You can put the multi in a number of places:
 * - In a .drush folder in your HOME folder.
 * - Along with one of your existing modules. If your command is
 *   related to an existing module, this is the preferred option.
 * - In a folder specified with the include option.
 * - In /path/to/drush/commands (not a "Smart Thing to Do", but it would work).
 */

/**
 * @page cmds Commands  
 * Drush_multi provides the following commands:
 * 
 * @subpage multi_status_help "multi status"
 * 
 * @subpage multi_create_help "multi create"
 * 
 * @subpage multi_exec_help "multi exec"
 * 
 * @subpage multi_drupalupdate_help "multi drupalupdate"
 * 
 * @subpage multi_sql_dump_help "multi sql dump"
 * 
 * @subpage multi_nagios_help "multi nagios"
 * 
 * @subpage multi_site_help "multi site"
 */
 
/**
 * @page multi_status_help "multi status"
 * An extended drush core status
 * 
 * The core status is extended with 
 * 
 * 'Site Counter', number of sites within the installation
 * 
 * 'Sites', sites within the installation 
 * 
 * 'Prev.Installations', previous installed Drupal versions 
 * 
 * 'Drupal Core', Status of Drupal core
 *  
 * @section Aliases
 * @subsection multi_status_alias_mst mst  
 * @section Examples
 * @code
 * drush -r /path/to/drupal multi status
 * @endcode
 * We asume that we are running drupal-6-14 with the 2 sites, default and foobar.com
 * and drupal-6.11, drupal-6.12 and drupal-6.13 before drupal-6.14.
 * @code'
 * PHP configuration : /etc/php5/cli/php.ini
 * Drupal Root       : /var/www/6.x
 * Drupal version    : 6.14
 * Site Path         : sites/default
 * Site URI          : http://default
 * Database Driver   : mysqli
 * Database Hostname : localhost
 * Database Username : dbuser
 * Database Name     : db
 * Database Password : dbpasswd
 * Database          : Connected
 * Drupal Bootstrap  : Successful
 * Drupal User       : Anonymous
 * Site Counter      : 2
 * Sites             : foobar.com default
 * Prev.Installations: drupal-6.11 drupal-6.12 drupal-6.13
 * Drupal Core       : Uptodate
 * '@endcode
 * If drupal-6.15 is available, in the "Drupal Core" section will display a red
 * @code
 * '6.15 available'
 * @endcode 
 */

/** 
 * @page multi_create_help "multi create"
 * Create a Drupal installation with the symbolic link structure as described in the \ref intro.
 * @section Aliases
 * @subsection multi_create_alias_mcr mcr
 * @subsection multi_create_alias_create create
 * @section Arguments
 * @subsection destination /path/to/installation
 * The path where the installation should be created
 * @subsection core Core compatibility
 * Core compatibility e.g. 6 for drupal 6
 * @section Examples
 * @code
 * drush multi create /var/www 6
 * @endcode
 * Create a drupal 6 installation in /var/www 
 * with creating the directories 6.x_backup, 6.x_profiles 
 * and 6.x_sites in /var/www link them as backup, profiles and sites  
 * into the drupal installation folder with a relative path.
 */   
   
/** 
 * @page multi_exec_help "multi exec"
 * Perform a drush core command on all sites (batch mode).
 * @section Aliases
 * @subsection multi_exec_alias mex
 * @section Arguments
 * @subsection multi_exec_command command
 * The drush command to execute. 
 * For drush commands with blanks like 'watchdog show'
 * make sure to use single or double quotes. Mandatory.
 * @section Options
 * @subsection multi_exec_option --option
 * Option to pass to drush command. e.g. 
 * '--limit=20' for 'watchdog show'. 
 * The number of messages to show. Optional.
 * @subsection multi_exec_argument --argument
 * Argument to pass to drush command e.g. 'php'
 * as type for 'watchdog delete' and  'watchdog show'. Optional.
 * @section Examples
 * @code
 * drush -r /path/to/drupal multi exec cron
 * @endcode
 * Run all cron hooks on all sites in your multisite installation.  
 * @code 
 * drush -r /path/to/drupal multi exec 'watchdog show' --option='--limit=30' --argument=php
 * @endcode
 * Shows the 30 newest log entries with facility 'php' 
 * @code
 * drush -r /path/to/drupal multi exec updatedb
 * @endcode
 * Execute the update.php process for all sites.
 * @code
 * drush -r /path/to/drupal multi cache clear
 * @endcode
 * Flushes the cache for all sites
 */ 

/**
 * @page multi_drupalupdate_help "multi drupalupdate"
 * Update your installation if there is a new minor release available,
 * assuming the same structure as in the @ref intro for the installation.
 * 
 * @note The Drupal root specified as /path/to/drupal must be a symbolic link.
 * @section Aliases
 * @subsection multi_drupalupdate_alias_mdr mdr
 * @subsection multi_drupalupdate_alias_drupalupdate drupalupdate
 * @section Options
 * @subsection multi_drupalupdate_sql_dump_option --sql-dump
 * Executes 'multi sql dump' before running 'multi drupalupdate'. 
 * 
 * See @ref multi_sql_dump_help for usable options and details.
 * 
 * @subsection multi_drupalupdate_updatedb_option --updatedb
 * Executes 'updatedb' after ececution of 'multi drupalupdate' via @ref multi_exec_help.
 * 
 * @section Examples
 * @code
 * drush -r /path/to/drupal multi drupalupdate
 * @endcode
 * Runs a 'multi drupalupdate'on /path/to/drupal 
 * 
 * @code
 * drush -r /path/to/drupal multi drupalupdate --sql-dump --comment='before drupaldate' --update
 * @endcode
 * Runs a 'multi drupalupdate'on /path/to/drupal with
 * 
 * making sql dumps of all sites with optional comment 'before drupalupdate' for sql dump filenames
 * 
 * and execution of 'updatedb' afterwards 
 *  
 */
   
/** 
 * @page multi_sql_dump_help "multi sql dump"
 * Perform an sql dump on all sites (batch mode).
 * @section Aliases
 * @subsection multi_sql_dump_alias_msd msd
 * @section Options
 * @subsection multi_sql_dump_destination --target
 * Absolute Path to the directory where to store the sql dumps. 
 * Default /tmp.
 * @subsection multi_sql_dump_comment --comment
 * Comment for filename. May contain alphanumics, '-' and '_'.
 * @subsection multi_sql_dump_bzip2 --bzip2
 * Use bzip2 for compression of each sql dump.
 * @section Examples
 * @code
 * drush -r /path/to/drupal multi sql dump --bzip2 --comment=before-update-to-6.15 --target=$HOME/db_backups
 * @endcode
 * Run 'sql dump' on all sites with comment 
 * 'before-update-to-6.15' as comment for each  result file.
 *
 * The result files are bzip2 compressed and stored in 'db_backups' in your home dirctory. 
 * 
 * Dump name schema for the command as executed above:
 * 
 * '${uri}'_before-update-to-6.1_{date('Y-m-d\TH:i')}.sql.bz2', where ${uri} is a site name.
 */

/**
 * @page multi_nagios_help "multi nagios"
 * For the use as nagios plugin. Print a message and returns with exit status 2 (Critical)
 * if updates available.
 * 
 * multi_nagios could be run on the with -r /--root supplied Drupal installation
 * to check for core updates.
 * 
 * or
 * 
 * on a the with -l / --uri supplied site to check for core and module updates
 * 
 * @note If there is a site at sites/default, this command runs on site basis, although you have only specified the Drupal installation.
 * 
 * @section Aliases
 * @subsection multi_nagios_alias_mna mna
 * @subsection multi_nagios_alias_nagios nagios
 * @section Options
 * @subsection multi_nagios_file --file
 * Path to release information file, this could
 * be a (temp) file  to avoid nrpe-socket timeouts. 
 * It is also possible to supply path via URI, 
 * like http://updates.drupal.org/release-history/drupal/6.x.
 * 
 * @note This option could only be used to check the Drupal installation.
 * @section Examples
 * @subsection nagios_root Checking the Drupal installation for core updates
 * @code
 * drush -r /path/to/drupal multi nagios
 * @endcode
 * We assume that drupal-6.14 is the recommended release.
 * With an older version, eg. drupal-6.13 we get:
 * @code
 * DRUPAL ROOT CRITICAL: drupal-6.13 @ /path/to/drupal
 * - drupal-6.14 (2009-09-16 21:40) available, see http://drupal.org/node/579476 for details.
 * @endcode
 * with exit status 2 
 * 
 * Or if we are running the recommended version:
 * @code
 * DRUPAL ROOT OK: drupal-6.14 @ /path/to/drupal/6.x is uptodate.
 * @endcode
 * with exit status 0.
 * 
 * @code
 * drush -r /path/to/drupal multi nagios --file=/path/to/file
 * @endcode
 * Same as above with a specified file.
 * 
 * 
 * @subsection nagios_site Checking a site on core and module updates
 * @code
 * drush -r /path/to/drupal -l example.com multi nagios
 * @endcode
 * If there are no updates available, this will produce to following output:
 * @code
 * DRUPAL SITE OK - example.com @ /path/to/drupal/example.com: All modules are uptodate.
 * @endcode
 * 
 * If there are available updates, it might produce the following output:
 * @code
 * DRUPAL SITE CRITICAL - example.com @ /path/to/drupal/sites/example.com: there are updates for admin_menu,drupal,image, see http://example.com/?q=admin/reports/updates for datails.
 * @endcode 
 */

/** 
 * @page multi_site_help "multi site"
 * Create a new site in your your multisite installation within a files, modules and themes directory.
 * @section Aliases 
 * @subsection multi_nagios_site_alias_msi msi
 * @subsection multi_nagios_site_alias_site site
 * @section Arguments
 * @subsection multi_nagios_site_name site-name
 * Name of the site. Mandatory.
 * @section Options
 * @subsection multi_site_create_dirs --create-directories
 * Comma seperated list of aditional directories.
 * @section Examples
 * @code
 * drush -r /path/to/drupal multi site example.com --create-directories=dir1,dir2,dir3
 * @endcode
 * Create the site 'example.com' within a settings.php, a 'files', 'themes', 'modules' directory
 * and the additional directories 'dir1', 'dir2' and 'dir3' in /path/to/drupal/sites.
 */   
?>