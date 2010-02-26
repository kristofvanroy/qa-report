<?php

/**
 * Detail page callback
 */
function qa_report_issue_detail_page($arg){
  $output = l(t('Go to issue overview'), 'admin/qa/issues').'<br />';

  $result = db_query("SELECT i.description, i.created, p.path, s.browser, s.version, s.platform
    FROM {qa_report_issues} i
    LEFT JOIN {qa_report_paths} p ON i.pid = p.pid
    LEFT JOIN {qa_report_setups} s ON i.sid = s.sid
    WHERE id = %d", $arg);
  while($row = db_fetch_object($result)){
    $data = $row;
  }

  if(!empty($data)){
    $row[] = array(t('Created'), format_interval(time() - $data->created).' '.t('ago'));
    $row[] = array(t('Location'), l($data->path, $data->path));
    $row[] = array(t('Description'), check_plain($data->description));
    $row[] = array(t('Browser'), check_plain($data->browser. ' '.$data->version));
    $row[] = array(t('Platform'), check_plain($data->platform));
    $row[] = array('', l('Go directly to issue', $data->path));

    return $output.theme('table', $header, $row);
  }
  return $output.t('No issue found');
}

/**
 * Return all issue
 */
function qa_report_get_issue_list() {
  $result = pager_query(db_rewrite_sql('SELECT i.description, i.created, i.id, p.path FROM {qa_report_issues} i LEFT JOIN {qa_report_paths} p ON p.pid = i.pid'), 50, 0, NULL);

  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  );
  $options = array();
  /*foreach (module_invoke_all('node_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }*/



  $options['jira'] = t('Send to jira');
  $options['jira_resolve'] = t('Close and resolve on Jira');


  $form['options']['operation'] = array(
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => 'approve',
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    //'#submit' => array('node_admin_nodes_submit'),
  );

  while ($issue = db_fetch_object($result)) {
    $issues[$issue->id] = '';
    $form['description'][$issue->id] = array('#value' => $issue->description);
    $form['created'][$issue->id] =  array('#value' => format_interval(time() - $issue->created).' '.t('ago'));
  }

  $form['issues'] = array('#type' => 'checkboxes', '#options' => $issues);
  $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));
  $form['#theme'] = 'qa_report_issue_list';

  return $form;
}

function qa_report_get_issue_list_submit(&$form, &$form_state){
  $values = $form_state['values'];
  module_invoke_all('qr_operations','create_issue',$values);
}