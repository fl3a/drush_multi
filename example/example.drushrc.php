<?php
// $Id: 

/*
 * Examples of valid drush_multi statements for a drushrc.php file. 
 * Use this file to cut down on typing of options and avoid mistakes.
 *
 * Append this file to drushrc.php. drushrc.php should be in one of
 * five convenient places, listed below in order of precedence:
 *
 * 1. Drupal site folder (e.g sites/{default|example.com}/drushrc.php).
 * 2. Drupal installation root.
 * 3. In any location, as specified by the --config (-c) option.
 * 4. User's .drush folder (i.e. ~/.drush/drushrc.php).
 * 5. System wide configuration folder (e.g. /etc/drush/drushrc.php).
 * 6. Drush installation folder.
 *
 * If a configuration file is found in any of the above locations, it
 * will be loaded and merged with other configuration files in the
 * search list.
 *
 * IMPORTANT NOTE on configuration file loading:
 *
 * At its core, drush works by "bootstrapping" the Drupal environment
 * in very much the same way that is done during a normal page request
 * from the web server, so most drush commands run in the context
 * of a fully-initialized website.
 *
 * Configuration files are loaded in the reverse order they are
 * shown above.  Configuration files #6 through #3 are loaded immediately;
 * the configuration file stored in the Drupal root is loaded
 * when Drupal is initialized, and the configuration file stored
 * in the site folder is loaded when the site is initialized.
 *
 * This load order means that in a multi-site environment, the
 * configuration file stored in the site folder will only be
 * available for commands that operate on that one particular
 * site.  Additionally, there are some drush commands such as
 * pm-download do not bootstrap a drupal environment at all,
 * and therefore only have access to configuration files #6 - #3.
 * The drush commands 'rsync' and 'sql-sync' are special cases.
 * These commands will load the configuration file for the site
 * specified by the source parameter; however, they do not
 * load the configuration file for the site specified by the
 * destination parameter, nor do they load configuration files
 * for remote sites.
 */

/** @todo Implement that multi-site works with "apache-user" option from commandline too */
// (APACHE_RUN_USER) Used to set permissions to the webserver user (multi-site)
// for settings.php during the installation and for the files directory
// $options['multi-apache-user'] = 'www-data';

/** @todo Implement that multi-site works with "apache-group" option from commandline too */
// (APACHE_RUN_GROUP) Used to set permissions to the webserver group (multi-site)
// for settings.php during the installation and for the files directory
// $options['multi-apache-group'] = 'www-data';

/** @todo Implement that multi-sql-dump works with option "multi-sql-dump-target" */
/** @todo Rename --target option to something else, target is used by drush itself */
// Target directory used for writing the result files to (multi-sql-dump)
// $options['multi-sql-dump-target'] = '/path/where/to/store/the/sqldumps';

/** @todo Implement that multi-sql-dump works with option "multi-sql-dump-bzip2" */
// Use bzip2 for the compression of the result files (multi-sql-dump)
// $options['multi-sql-dump-bzip2'] = TRUE;

/** @todo Implement that multi-sql-dump works with option "multi-sql-dump-datetime" */
// Use datetime strings in result file names (multi-sql-dump)
// $options['multi-sql-dump-datetime'] = TRUE;

/** @todo Implement that multi-site works with option "multi-site-create-directories */
// List of optional directories created within a site directory (multi-site)
// $options['multi-site-create-directories'] = 'modules/patched,foo,foobar';


