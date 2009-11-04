 // $Id$        

 * Notes
 * Introduction
 * Requirements
 * Installation
 * Commands 
 * Documentation
 * FAQ

 
NOTES
-----
 
This command still under development. Not for use on production sites. Use at own risk!

Before running, make backups of your sites and data! 
Try a dry run with -s / --simulate and -v / --verbose options first to see what will happen.
 
 
INTRODUCTION
------------

Current Maintainer: fl3a <http://drupal.org/user/51103> 
Original Sponsor: ISL Individuelle System Lösungen <http://is-loesungen.de>

Credits to the mighty hundfred, friend and Bastard Operator from Hell.

I use the following symbolic links structure on our servers and wrote a bash script for handling drupal updates years ago.

Within this structure and this drush command, it is now possible to do that via drush.

Now can drush update drupal. 

Here is the very specific structure this command deals with for multi drupalupdate and multi create:

 drupal/
 |-- 6.x -> drupal-6.14
 |-- 6.x_backup
 |-- 6.x_sites
 |   |--all
 |   |   |-- modules
 |   |   `-- themes
 |   |-- default
 |   `-- example.com
 |      |-- files
 |      |-- modules
 |      `-- themes
 `-- drupal-6.14
     |-- backup -> ../6.x_backup
     |-- includes
     |-- misc
     |-- modules
     |-- profiles
     |-- scripts
     `-- sites  -> ../6.x_sites


INSTALLATION
------------

You can put the multi in a number of places:

  - In a .drush folder in your HOME folder.
  - Along with one of your existing modules. If your command is
    related to an existing module, this is the preferred option.
  - In a folder specified with the include option.
  - In /path/to/drush/commands (not a "Smart Thing to Do", but it would work).

REQUIREMENTS
------------

Since this is a drush command, you need drush.
For the installation of drush see the drush README.txt 
at http://cvs.drupal.org/viewvc/drupal/contributions/modules/drush/README.txt?revision=1.29&view=markup  
  
COMMANDS
--------
  
 * multi status
  Extends the drush core status with 
  sites, sitescounter, previous installations and drupal version status
    
 * multi create
  Create a Drupal installation with the symbolic link structure as above.
  
 * multi drupalupdate
  Performs a drupal minor version update.
  
 * multi exec
  Perform a drush core command on all sites.
  
 * multi sql dump
  Perform an sql dump on all sites.
  
 * multi nagios
  Command for use as nagios plugin with exit status and status message.   
  
 * multi site
  Create new site into the sites folder.  

DOCUMENTATION
-------------

Each multi command got a help.
  
To get specific help, type:
drush help multi ${multicommand} 

Since the the code is commented with doxygen 
you can find the documentation at 
http://is-loesungen.de/docu/multi/.
    
FAQ
---
 Q: Why the name multi?
 A: Because this drush command does operations on/with multisites.
  
 Q: Who or what is hundfred?
 A: Hundfred is ketchup, mayonnaise and mustard, full steam ahead towards who cares, or a security risk.