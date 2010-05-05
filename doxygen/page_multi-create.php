<?php 
// $Id:

/**
 * @file 
 * Page multi-create
 * 
 * Additional Doxygen documentation 
 */

/** 
 * @page multi_create_help "multi-create"
 * Create a Drupal installation with the symbolic link structure as described in the \ref intro.
 * @section Aliases
 * @subsection multi_create_alias_mcr mcr
 * @subsection multi_create_alias_create create
 * @section Arguments
 * @subsection destination /path/to/installation
 * The path where the installation should be created
 * @section Options
 * @subsection multi_create_makefile --makefile
 * Path to drush_make makefile.
 * 
 * The makefile must contain the core specification (eg. <em>core = 6.x</em> for Drupal 6)
 * and Drupal as project (<em>projects[] =  drupal</em>).
 * @subsection multi_create_core --core
 * Drupal core version (5.x, 6.x). 
 * 
 * Take defaults from pm_parse_project_version(array('drupal') if not supplied.
 * @section Examples
 * @code
 * drush multi-create /var/www --core=6.x
 * @endcode
 * Create a Drupal 6 installation in /var/www 
 * with creating the directories 6.x_backup, 6.x_profiles 
 * and 6.x_sites in /var/www link them as backup, profiles and sites  
 * into the drupal installation folder with a relative path.
 * @code
 * drush multi-create /var/www --makefile=/path/to/makefile.make
 * @endcode
 * Creates a Drupal multisite installation in /var/www by using the supplied
 * makefile at /path/to/makefile.make.
 */   