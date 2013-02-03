<?php

function user_stats_dash() {
    drupal_set_html_head('<script src="https://www.google.com/jsapi"></script>');
    drupal_add_js('misc/jquery.js', 'core', 'header');
    extract(user_stats_filters());
    $user_registartions_count = user_stats_get_registration_counts($fdate, $tdate);
    $output = drupal_get_form('user_stats_filter_form');
    $output.= theme('user_stats_dash', json_encode($user_registartions_count));
    return $output;
}


function user_stats_get_registration_counts($fdate='', $tdate='') {
    $where = '';

    $args = array();
    if ($fdate) {
        $where.=" AND DATE(FROM_UNIXTIME(created))>=DATE('%s')";
        $args[] = $fdate;
    }

    if ($tdate) {
        $where.=" AND DATE(FROM_UNIXTIME(created))<=DATE('%s')";
        $args[] = $tdate;
    }
    $sql = db_rewrite_sql("SELECT COUNT(uid) AS count,
				DATE(FROM_UNIXTIME(created)) AS date FROM {users} WHERE uid>1 $where GROUP BY date", 'user', 'uid');
    $results = db_query($sql, $args);
    $data = array();
    while ($row = db_fetch_array($results)) {
        $data[] = $row;
    }
    return $data;
}

function user_stats_export_as_csv($filename, $csv_array) {

    header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
    header('Content-Type: text/csv');
    header("Content-Disposition: attachment; filename=\"$filename.csv\"");

    $fp = fopen('php://output', 'w');
    foreach ($csv_array as $line) {
        fputcsv($fp, $line);
    }
    fclose($fp);
    exit();
}

function user_stats_export() {
    extract(user_stats_filters());
    $args = array();
    if ($fdate) {
        $where.=" AND DATE(FROM_UNIXTIME(created))>=DATE('%s')";
        $args[] = $fdate;
    }

    if ($tdate) {
        $where.=" AND DATE(FROM_UNIXTIME(created))<=DATE('%s')";
        $args[] = $tdate;
    }
    $sql = db_rewrite_sql("SELECT name, mail,DATE(FROM_UNIXTIME(created)) AS date FROM {users} WHERE uid>1 $where  ORDER BY created DESC", 'user', 'uid');
    $results = db_query($sql, $args);
    $data[0] = array('Username','Mail','Date');
    while ($row = db_fetch_array($results)) {
        $data[] = $row;
    }

    user_stats_export_as_csv("user_data_" . date("y_m_d", strtotime($fdate)) . "_" . date("y_m_d", strtotime($tdate)), $data);
}

function user_stats_filter_form() {
    $session = $_SESSION['user_stats_filter'];
    $session = is_array($session) ? $session : array();

    extract(user_stats_filters());

    $format = 'Y-m-d';
    $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Show only users where')
    );

    $form['filter']['fdate'] = array(
    '#type' => 'date_select',
    '#title' => t('from'),
    '#default_value' => $fdate,
    '#date_format' => $format,
    );

    $form['filter']['tdate'] = array(
    '#type' => 'date_select',
    '#title' => t('to'),
    '#default_value' => $tdate,
    '#date_format' => $format,
    );

    $form['filter']['buttons']['submit'] = array(
    '#type' => 'submit',
    '#prefix' => '<br /><div class="container-inline" id="user-admin-buttons">',

    '#value' => t('Filter'),
    );
    $form['filter']['buttons']['export'] = array(
    '#type' => 'submit',
    '#suffix' => '</div>',
    '#value' => t('Export'),
    );
    return $form;
}

function user_stats_filter_form_submit($form, &$form_state) {
    $op = $form_state['values']['op'];
    switch ($op) {
        case t('Filter'): case t('Export'):
            if (isset($form_state['values'])) {
                $_SESSION['user_stats_filter'] = array('fdate'=>$form_state['values']['fdate'],'tdate'=>$form_state['values']['tdate']);
            }
            break;
    }
    if ($op == t('Export')) {

        $form_state['redirect'] = 'user_stats/export';
    } else {

        $form_state['redirect'] = 'admin/user/user/report';
    }

}

function user_stats_filter_form_validate($form, &$form_state) {

    if (strtotime($form_state['values']['tdate']) <= strtotime($form_state['values']['fdate'])) {
        form_set_error('', t('You must select a valid from date'));
    }
}

function user_stats_filters() {
    if (isset($_SESSION['user_stats_filter'])) {
        extract($_SESSION['user_stats_filter']);
    } else {
        $fdate = date("Y-m-d", strtotime(date("Y-m-d")) - (60 * 60 * 24 * 90));
        $tdate = date("Y-m-d");
    }

    return compact('fdate', 'tdate');
}