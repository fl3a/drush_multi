<?php 
// $Id:

/**
 * @file 
 * Page Introduction
 * 
 * Additional Doxygen documentation 
 */

/** 
 * @page intro Introduction
 * 
 * I use the following symbolic links structure on our servers and wrote a bash script for handling Drupal updates years ago.
 * 
 * Within this structure and this drush command, it is now possible to do that via drush.
 * 
 * Now drush can update Drupal. 
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
 */
