<?php 
// $Id:

/**
 * @file 
 * Page multi-nagios
 * 
 * Additional Doxygen documentation 
 */

/**
 * @page multi_nagios_help "multi-nagios"
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
 * drush -r /path/to/drupal multi-nagios
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
 * drush -r /path/to/drupal multi-nagios --file=/path/to/file
 * @endcode
 * Same as above with a specified file.
 * 
 * 
 * @subsection nagios_site Checking a site on core and module updates
 * @code
 * drush -r /path/to/drupal -l example.com multi-nagios
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
