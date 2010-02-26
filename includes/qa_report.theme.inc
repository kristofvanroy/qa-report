<?php

function theme_qa_report_issue_list($form){
  $has_issues = isset($form['description']) && is_array($form['description']);
  $select_header = $has_issues ? theme('table_select_header_cell') : '';

  $header = array($select_header, t('Description'), t('Created'));
  $output = '';

  $output .= drupal_render($form['options']);

  if ($has_issues) {
    foreach (element_children($form['description']) as $key) {
      $row = array();
      $row[] = drupal_render($form['issues'][$key]);
      $row[] = drupal_render($form['description'][$key]);
      $row[] = drupal_render($form['created'][$key]);
      $rows[] = $row;
    }
  }
  else {
    $rows[] = array(array('data' => t('No posts available.'), 'colspan' => '3'));
  }

  $output .= theme('table', $header, $rows);
  if ($form['pager']['#value']) {
    $output .= drupal_render($form['pager']);
  }

  $output .= drupal_render($form);

  return $output;

}