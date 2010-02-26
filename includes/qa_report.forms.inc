<?php

/**
 * Block callback
 */
function qa_report_block_content_form(){
  $panes = array();

  $panes['report'] = qa_report_issue_form() + array('#title' => t('Report issue'));
  $panes['issues'] = array(
    '#title' => t('Open issues for this page'),
    'list' => array('#type' => 'markup', '#value' => qa_report_get_page_issue_list()),
  );
  $panes['stats'] = array(
    '#title' => t('Browser stats'),
    'list' => array('#type' => 'markup', '#value' => qa_report_get_stats(FALSE)),
  );

  if (!empty($panes)) {
    $panes['#type'] = 'admin_panes';
    return array('panes' => $panes);
  }
  return array();
}

/**
 * Issue form
 */
function qa_report_issue_form() {
  global $user;
  $form = array();

  $form['pid'] = array('#type' => 'value', '#value' =>  qa_report_set_path());
  $form['sid'] = array('#type' => 'value', '#value' =>  qa_report_set_setup());
  $form['uid'] = array('#type' => 'value', '#value' =>  $user->uid);
  $form['created'] = array('#type' => 'value', '#value' =>  time());
  $form['type'] = array('#type' => 'value', '#value' => 1);

  // Fetch browser data
  $data =  qa_report_get_client_info();

  $form['version'] = array(
    '#type' => 'item',
    '#title' => t('Version'),
    '#value' => $data->browser.' '.$data->version.' ('.$data->platform.')',
  );

  $form['path'] = array(
    '#type' => 'item',
    '#title' => t('Path'),
    '#value' => drupal_get_path_alias($_GET['q']),
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#rows' => 3,
    '#required' => true,
  );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Report'));

  return $form;
}

/**
 * Issue form validation handler
 */
function qa_report_block_content_form_validate(&$form, &$form_state) {
  //$values = $form_state['values'];
}

/**
 * Issue form submit handler
 */
function qa_report_block_content_form_submit(&$form, &$form_state) {
  drupal_write_record('qa_report_issues', $form_state['values']);
}