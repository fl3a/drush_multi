<?php 
// $Id:

/**
 * @file 
 * Page multi-exec
 * 
 * Additional Doxygen documentation 
 */


/** 
 * @page multi_exec_help multi-exec
 * Perform a drush core command on all sites (batch mode).
 * @deprecated @deprecated Similar functionality is in drush core 3.x, see http://drupal.org/node/652778 for details.
 * @deprecated Please use <code>drush -r /path/to/drupal \@sites ${command}</code> instead!
 * @deprecated <strong>This command will be removed soon.</strong>
 * @section Aliases
 * @subsection multi_exec_alias mex
 * @section Arguments
 * @subsection multi_exec_command command
 * The drush command to execute. 
 * For drush commands with blanks like 'watchdog show'
 * make sure to use single or double quotes. Mandatory.
 * @section Options
 * @subsection multi_exec_argument --argument
 * Argument to pass to drush command e.g. 'site_offline 1'
 * as argument for 'variable-set'. Optional.
 * @subsection multi_exec_option --option
 * Option to pass to drush command. or drush itself e.g. 
 * '--nocolor' to suppress color highlighting on log messages. Optional.
 * @section Examples
 * @code
 * drush -r /path/to/drupal multi-exec cron
 * @endcode
 * Run all cron hooks on all sites in your multisite installation.  
 * @code 
 * drush -r /path/to/drupal multi-exec variable-set --argument='site_offine 1' --option='--nocolor'
 * @endcode
 * Put all sites in maintenance mode without using color highlighting on log messages.
 * @code
 * drush -r /path/to/drupal multi-exec updatedb
 * @endcode
 * Execute the update.php process for all sites.
 * @code
 * drush -r /path/to/drupal multi-exec cache-clear
 * @endcode
 * Flushes the cache for all sites
 */ 
