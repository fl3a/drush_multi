<?php 
// $Id:

/**
 * @file 
 * Page multi-site
 * 
 * Additional Doxygen documentation 
 */


/** 
 * @page multi_site_help "multi-site"
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
 * drush -r /path/to/drupal multi-site example.com --create-directories=dir1,dir2,dir3
 * @endcode
 * Create the site 'example.com' within a settings.php, a 'files', 'themes', 'modules' directory
 * and the additional directories 'dir1', 'dir2' and 'dir3' in /path/to/drupal/sites.
 */