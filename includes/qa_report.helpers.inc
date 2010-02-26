<?php

/**
 * This module checks if a given path is already in our database
 * and creates a new entry if the path does not exist
 */
function qa_report_set_path(){
  static $pid;

  if(isset($pid)){
    return $pid;
  }
  else {
    $q = $_GET['q'];

    $return = menu_execute_active_handler();

    if (is_int($return)) {
      switch ($return) {
        case MENU_NOT_FOUND:
        case MENU_ACCESS_DENIED:
        //case MENU_SITE_OFFLINE:
          return 0;
        break;
      }
    }

    $result = db_result(db_query("SELECT pid FROM {qa_report_paths} WHERE path = '%s' AND status = 1", $q));

    if (isset($result) && is_numeric($result)){
      $pid = $result;
      return $pid;
    }
    else {
      $path->path = $q;
      drupal_write_record('qa_report_paths', $path);
      $pid = db_last_insert_id('qa_report_paths','pid');
      return $pid;
    }
  }
}

/**
 * This function will add a tracking record if it does not exits
 * and it will update and existing tracking record with a count and date
 */
function qa_report_set_tracking(){

  // Register the path
  if(qa_report_set_path()) {
    $data->pid = qa_report_set_path();

    // Get browser info
    $data->sid = qa_report_set_setup();

    // Last entry
    $data->last = time();

    $count = db_result(db_query("SELECT count FROM {qa_report_tracking} WHERE sid = %d AND pid = %d",
      $data->sid,
      $data->pid
      ));

    if($count > 0){
      $data->count = $count + 1;
      drupal_write_record('qa_report_tracking', $data, array('pid','sid'));
      return;
    }

    // Create first entry
    $data->first = $data->last;

    drupal_write_record('qa_report_tracking', $data);
  }
}

/**
 * Register or return (an existing) client setup
 */
function qa_report_set_setup(){
  static $sid;

  if (isset($sid)) {
    return $sid;
  }
  else {
    // Get data from the client
    $data = qa_report_get_client_info();

    // Fetch sid
    $sid = db_result(db_query("SELECT sid FROM {qa_report_setups} WHERE browser = '%s' AND version = '%s' AND platform = '%s'", $data->browser, $data->version, $data->platform));

    if(empty($sid)){
      drupal_write_record('qa_report_setups', $data);
      $sid = db_last_insert_id('qa_report_setups','sid');
    }
  }

  return $sid;
}

/**
 * Get the client browser, browser version and platform info
 */
function qa_report_get_client_info(){
  static $client_info;

  if (isset($client_info)) {
    return $client_info;
  }
  else {
    include_once('Browser.php');

    $browser = new Browser();

    if ($browser->getBrowser()) {
      $client_info->browser = $browser->getBrowser();
      $client_info->version = $browser->getVersion();
      $client_info->platform = $browser->getPlatform();
    }
  }

  return $client_info;
}

/*
 * This return a table with the statistics of the current page
 */
function qa_report_get_stats(){
  $pid = qa_report_set_path();
  $result = db_query("SELECT s.browser, s.version, s.platform, t.count
    FROM {qa_report_tracking} t
    LEFT JOIN {qa_report_setups} s ON t.sid = s.sid
    WHERE pid = %d", $pid);

  $header = array(t('Browser'),t('Version'),t('Count'),t('Platform'));
  while($row = db_fetch_object($result)){
    $rows[] = array($row->browser, $row->version, $row->count, $row->platform);
  }

  return theme('table', $header, $rows);
}

/**
 * Return all issues based upon the current page
 */
function qa_report_get_page_issue_list(){
  $result = db_query("SELECT id, description FROM {qa_report_issues} WHERE pid = %d ORDER BY created DESC", qa_report_set_path());

  $header = array(t('Description'));

  while($row = db_fetch_object($result)){
    $rows[] = array(l(truncate_utf8($row->description, 35, FALSE, TRUE),'admin/qa/issue/'.$row->id));
  }

  if(empty($rows)) return t('No issues for this page found');

  return theme('table', $header, $rows);
}