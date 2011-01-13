<?php 
// $Id:

/**
 * @file 
 * Page multi-nagios
 * 
 * Additional Doxygen documentation 
 */

/**
 * @page multi_nagios_help multi-nagios
 * For the usage as nagios plugin: 
 * 
 * Monitores a Drupal installation or a Drupal site and print a message and return with an exit status
 *  
 * @section multi_nagios_root Monitoring of a Drupal installation 
 * multi_nagios could be run on the with -r /--root supplied Drupal installation
 * to check for core updates.
 * 
 * If the Drupal installation is not up to date,
 * this command prints the recommended version,
 * the type of the update (Bug fixes or Security update), 
 * the release date and the link to the release note of the recommended version. 
 * 
 * <strong>Exit status</strong>
 *  
 * MULTI_NAGIOS_OK (0) if monitored Drupal is up to date
 *
 * or MULTI_NAGIOS_WARNING (1) if there is a 'Bug fixes' available
 * 
 * or MULTI_NAGIOS_CRITICAL(2) if there is a 'Security update' available
 *
 * @section multi_nagios_site Monitoring of a Drupal site 
 * Checks on the the with -l / --uri supplied site for core and module updates.
 * 
 * If the Drupal site is not up to date, 
 * this command will print the type of the update(Security update, update, unsupported release)
 * including the corresponding modules, Drupal core
 * and a link to the 'Available updates' page within drupal.
 * 
 * <strong>Exit status</strong>
 * 
 * MULTI_NAGIOS_OK (0) if monitored site is up to date
 *
 * or MULTI_NAGIOS_WARNING (1) if there are updates or unsupported modules available
 * 
 * or MULTI_NAGIOS_CRITICAL(2) if there are security updates available
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
 * drush -r /path/to/drupal/6.x multi-nagios
 * @endcode
 * We assume that drupal-6.14 is the recommended release. (6.14 was a security update)
 * 
 * With an older version, eg. drupal-6.13 we get:
 * @code
 * DRUPAL ROOT CRITICAL: drupal-6.13 @ /path/to/drupal/6.x
 * - drupal-6.14 (Security update) from 2009-09-16 21:40 available, see http://drupal.org/node/579476 for details.
 * @endcode
 * with exit status 2 
 * 
 * Or if we are running the recommended version: 
 * @code
 * DRUPAL ROOT OK: drupal-6.14 @ /path/to/drupal/6.x is up to date.
 * @endcode
 * with exit status 0.
 * 
 * @code 
 * drush -r /path/to/drupal/6.x multi-nagios --file=/path/to/file
 * @endcode
 * Same as above with a specified file.
 * 
 * 
 * @subsection nagios_site Checking a Drupal site on core and module updates
 * @code
 * drush -r /path/to/drupal/6.x -l example.com multi-nagios 
 * @endcode
 * Assuing we have got security updates, updates and a unsupported release of a module,
 * 
 * it will print the following output and exit with MULTI_NAGIOS_CRITICAL (2) because of the security update.
 * @code
 * DRUPAL SITE CRITICAL - example.com @ /path/to/drupal/6.x/sites/example.com: 
 *  SECURITY UPDATES available for image,lightbox2, Updates available for drupal,bueditor,imce, Installed version not supported: transliterations, see http://example.com/?q=admin/reports/updates for datails.
 * @endcode
 * 
 * If there are no updates available, this will produce to following output:
 * @code
 * DRUPAL SITE OK - example.com @ /path/to/drupal/6.x/sites/example.com: Everything is up to date.
 * @endcode 
 */
