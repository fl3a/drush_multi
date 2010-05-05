<?php 
// $Id:

/**
 * @file 
 * Page Installation
 * 
 * Additional Doxygen documentation 
 */

/** 
 * @page install Installation
 * You can put drush_multi in a number of places:
 * - In a .drush folder in your HOME folder.
 * - Along with one of your existing modules. If your command is
 *   related to an existing module, this is the preferred option.
 * - In a folder specified with the include option.
 * - In /path/to/drush/commands (not a "Smart Thing to Do", but it would work).
 * 
 * Change / cd to one of that places above and type the following command to get the latest Version (HEAD) from cvs:
 * @code
 * cvs -z6 -d:pserver:anonymous:anonymous@cvs.drupal.org:/cvs/drupal-contrib checkout -d drush_multi-HEAD contributions/modules/drush_multi/
 * @endcode
 */