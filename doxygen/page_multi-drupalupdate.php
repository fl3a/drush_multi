<?php 
// $Id:

/**
 * @file 
 * Page multi-drupalupdate
 * 
 * Additional Doxygen documentation 
 */

/**
 * @page multi_drupalupdate_help multi-drupalupdate
 * Update your installation if there is a new minor release available,
 * assuming the same structure as in the @ref intro for the installation.
 * 
 * All symbolic links within the original drupal installation are detected and preserved automatically.
 * @see @ref symlinks for handling and preserving symbolic links.
 * 
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
 * Runs a 'multi-drupalupdate'on /path/to/drupal
 *  
 * @code
 * drush -r /path/to/drupal multi-drupalupdate --sql-dump --comment='before drupaldate' --updatedb
 * @endcode
 * Runs a 'multi-drupalupdate'on /path/to/drupal with
 * 
 * making sql dumps of all sites with optional comment 'before drupalupdate' for sql dump filenames
 * 
 * and execution of 'updatedb' afterwards 
 * @note The Drupal root specified as /path/to/drupal must be a symbolic link. 
 */
