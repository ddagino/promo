<?php

/**
 * @file
 * Page callbacks for the admin of promos.
 */

/**
 * Implements hook_form().
 */
function promo_form($form, &$form_state, $promo = NULL){
  $form = array();

  $form['title'] = [
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Provide a name for this promo.'),
    '#default_value' => isset($promo->title) ? $promo->title : '',
    '#required' => TRUE,
    '#maxlength' => 255,
  ];

  $form['url'] = [
    '#type' => 'textfield',
    '#title' => t('Promo URL'),
    '#description' => t('An absolute URL that the banner image will link to when displayed. Relative URLs are not supported.'),
    '#default_value' => isset($promo->url) ? $promo->url: '',
    '#required' => TRUE,
  ];

  $form['fid'] = array(
    '#type' => 'managed_file',
    '#title' => t('Banner Image'),
    '#description' => t('The image that will display wrapped inside the promo URL.'),
    '#default_value' => isset($promo->fid) ? $promo->fid: '',
    '#required' => TRUE,
    '#upload_location' => 'public://promo_banner/',
  );

  field_attach_form('promo', $promo, $form, $form_state);

  // Pull out path and attributes to help with code readability.
  $delete_path = '';
  $delete_attributes = array();

  if (isset($promo->pid)) {
    $delete_path = "admin/content/promo/manage/$promo->pid/delete";
    $delete_attributes = array(
      'attributes' => array(
        'id' => array("promo-delete-$promo->pid"),
        'class' => array('button remove')
      ),
      'query' => array('destination' => 'admin/content/promo')
    );
  }

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => isset($promo->pid) ? t('Update Promo') : t('Save Promo'),
    ),
    'delete_link' => array(
      '#markup' => isset($promo->pid) ? l(t('Delete'), $delete_path, $delete_attributes) : '',
    ),
  );

  return $form;
}

/**
 * Implements hook_form_validate().
 */
function promo_form_validate($form, &$form_state) {
  if (!valid_url($form_state['values']['url'], TRUE)) {
    form_set_error('url', 'Please enter a valid URL.');
  }
}

/**
 * Implements hook_form_submit().
 */
function promo_form_submit(&$form, &$form_state) {
  $promo = entity_ui_form_submit_build_entity($form, $form_state);
  $promo->save();

  // By default managed_file fields are stored in the temp directory and are
  // eventually purged. Here we set our file's status to permanent and add it
  // to the file_usage table.
  $banner_image_file = file_load($form_state['values']['fid']);
  $banner_image_file->status = FILE_STATUS_PERMANENT;
  file_save($banner_image_file);
  file_usage_add($banner_image_file, 'promo', 'promo', $promo->pid);

  drupal_set_message(t('The promo: @title has been created.', array('@title' => $promo->title)));
  $form_state['redirect'] = 'admin/content/promo';
}
