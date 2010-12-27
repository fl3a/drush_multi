<?php 
// $Id:

/**
 * @file 
 * Page multi-status
 * 
 * Additional Doxygen documentation 
 */

/**
 * @page multi_status_help multi-status
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
 * drush -r /path/to/drupal multi-status
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
