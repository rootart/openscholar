<?php

class os_boxes_media extends os_boxes_default {

  public function options_defaults() {
    $options = parent::options_defaults();
    $options['fid'] = 0;
    return $options;
  }

  public function options_form(&$form_state) {
    $form = parent::options_form($form_state);

    $form['fid'] = array(
      '#title' => t('File or HTML'),
      '#type' => 'media',
      '#default_value' => array('fid' => $this->options['fid']),
      '#description' => t('Choose, enter or link to the media you wish to embed.')
    );

    $form['help'] = array(
      '#prefix' => '<p>',
      '#markup' => t('If you\'d like additional text around your image, video or iframe, you should use the Custom Text/HTML box. It will allow you to embed media around text.'),
      '#suffix' => '</p>',
    );

    return $form;
  }

  public function options_submit($form, &$form_state) {
    $old = $form['options']['fid']['#default_value']['fid'];

    if ($old != $this->options['fid']) {

      if (module_exists('vsite') && $space = spaces_get_space()) {
        $id = $space->group->nid;
      }

      if ($file = file_load($this->options['fid'])){
        file_usage_add($file, 'os_boxes', 'node', $id, 1);
      }

      if ($old && $old_file = file_load($old)) {
        file_usage_delete($old_file, 'os_boxes', 'node', $id, 1);
        file_delete($old_file);
      }
    }
  }

  public function render() {
    $block = parent::render();

    if ($file = file_load($this->options['fid'])) {
      $render = file_view($file, 'media_large');
      $block['content'] = drupal_render($render);
    }
    else {
      $block['content'] = t('No media given.');
    }
    return $block;
  }
}