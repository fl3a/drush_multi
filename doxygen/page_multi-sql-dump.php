<?php 
// $Id:

/**
 * @file 
 * Page multi-sql-dump
 * 
 * Additional Doxygen documentation 
 */

/** 
 * @page multi_sql_dump_help "multi-sql-dump"
 * Improved sql-dump.
 * @section Aliases
 * @subsection multi_sql_dump_alias_msd msd
 * @section Options
 * @subsection multi_sql_dump_destination --target
 * Absolute Path to the directory where to store the sql dumps. 
 * Default /tmp.
 * @subsection multi_sql_dump_dump_type --dump-type
 * Arguments:
 * 
 * "full" Creates one file for all tables (default).
 * 
 * "tables" Creates for each table within each database a seperate file.
 * 
 * @subsection multi_sql_dump_datetime --datetime
 * Adds a datetime to resultfiles. (YYYY-MM-DDThh:mm) 
 * @subsection multi_sql_dump_comment --comment
 * Comment for filename. May contain alphanumics, '-' and '_'.
 * @subsection multi_sql_dump_bzip2 --bzip2
 * Use bzip2 for compression of each sql dump.
 * @section Examples
 * @code
 * drush -r /path/to/drupal -l example.com  multi-sql-dump --bzip2 --comment=before-update-to-6.15 --target=$HOME/db_backups
 * @endcode
 * Run 'sql dump' on example.com with 'before-update-to-6.15' as comment.                     
 *
 * The result file will be bzip compressed and stored in 'db_backups' in your home directory.
 * 
 * Dump name schema:
 * @code
 * ${uri} [ _ ${comment} _ ${date('Y-m-d\TH:i')] .sql [.bz2]
 *                               
 * where ${url} is example.com supplied by -l/--uri.
 */
