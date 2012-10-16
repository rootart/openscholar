<?php

/**
 * @file imagefield_crop.inc
 *
 * Migration field handler for imagefield_crop
 *
 * @author sagotsky
 *
 */
class MigrateImagefieldcropFieldHandler extends MigrateImageFieldHandler {
  public function __construct() {
    // This handler will take care of all the simple core field types
    $this->registerTypes(array('imagefield_crop'));
  }

  // Here, we take a simple array of values (for an integer field, it might look like
  // array(35, -23, 85738)
  public function prepare($entity, array $field_info, array $instance, array $values, $langcode=NULL) {

    if (!isset($entity->og_group_ref) && in_array($entity->type, array('personal', 'project', 'department'))) {
      $gid = FALSE; //not attaching to a group
    } elseif (is_string($entity->og_group_ref)) {
      $gid = $entity->og_group_ref;
    } elseif (is_array($entity->og_group_ref)) {
      $gid = (isset($entity->og_group_ref[LANGUAGE_NONE][0]['value'])) ? $entity->og_group_ref[LANGUAGE_NONE][0]['value'] : $entity->nid;
    }

    if (!$values[0]) {
      return array();
    }

    //recycle previous thumbnail
    $action = $this->_get_action($values);

    if ($action == 'recycle') {
      if ($recycled = $this->_recycle($values[0], $gid, $entity->uid)) {
        $values[0] = $recycled->fid;
        $values['arguments']['file_class'] = 'MigrateFileFid';
      }
    } elseif ($action == 'crop') {
      //crop a new image
      if ($original = $this->_save_original($values[0], $gid, $entity->uid)) { //no og_group_ref yet....
        $this->_image_dimensions($values);
        $items = array('fid' => $original->fid); //also has cropbox_x and y widht and height
        $items += array_intersect_key($values['arguments'], array_flip(array('cropbox_x', 'cropbox_y', 'cropbox_width', 'cropbox_height')));
        if (count($items) >= 5) {
          $items = array($items);
          imagefield_crop_field_insert('node', $entity, $field_info, $instance, $langcode, $items);
        }

        //image was cropped.  instead of migrating the whole image, use MigrateFileFid to accept the existing fid.
        $values[0] = $original->fid;
        $values['arguments']['file_class'] = 'MigrateFileFid';

      }
    } else {
      //just keep the image as is
      $values[0] = $values[0];
    }

    $return = parent::prepare($entity, $field_info, $instance, $values);
    if (!count($return)) {
      return $return;
    }

    $migration = Migration::currentMigration();
    $arguments = (isset($values['arguments']))? $values['arguments']: array();
    unset($values['arguments']);
    // In Drupal 7, field values are indexed by language, which can be specified
    // at various levels - this member implemented in MigrateFieldHandler works
    // out which language to use, applying appropriate defaults
    $language = $this->getFieldLanguage($entity, $field_info, $arguments);
    // Setup the standard Field API array for saving.
    $delta = 0;
    foreach ($values as $value) {
      if ($value) {
        $return[$language][$delta]['value'] = $value;
        $delta++;
      }
    }
    if (!isset($return)) {
      $return = NULL;
    }
    return $return;
  }


  /**
   * Implementation of MigrateFileFieldBaseHandler::buildFieldArray().
   */
  protected function buildFieldArray($field_array, $arguments, $delta) {
    foreach (array('cropbox_x', 'cropbox_y', 'cropbox_height', 'cropbox_width') as $field) {
      if (isset($arguments[$field])) {
        if (is_array($arguments[$field])) {
          $field_array[$field] = $arguments[$field][$delta];
        }
        else {
          $field_array[$field] = $arguments[$field];
        }
      }
    }

    return parent::buildFieldArray($field_array, $arguments, $delta);
  }

  /**
   * Implementation of MigrateFieldHandler::fields().
   * Note that file and image fields support slightly different field lists.
   *
   * @param $type
   *  The file field type - 'file' or 'image'
   * @param $parent_field
   *  Name of the parent field.
   * @param Migration $migration
   *  The migration context for the parent field. We can look at the mappings
   *  and determine which subfields are relevant.
   * @return array
   */
  public function fields($type, $parent_field, $migration = NULL) {
    $fields = parent::fields($type, $parent_field, $migration);
    $fields += array(
      'cropbox_x' => t('Horizontal origin of crop.'),
      'cropbox_y' => t('Vertical origin of crop.'),
      'cropbox_height' => t('Height of selected section of image.'),
      'cropbox_width' => t('Width of selected section of image.'),
    );

    return $fields;
  }


  /**
   * @function _get_action
   *
   * Figure out if this image will be cropped in d7 or if we can reuse the cropped image from d6.
   */
  function _get_action($values) {
    $original_file = $values[0];

    //check for recycle
    $thumbnail_filepath = variable_get('os_migrate_files') . '/' . $original_file;
    $full_filepath = $thumbnail_filepath . '.crop_display.jpg';
    if (is_readable($thumbnail_filepath) && is_readable($full_filepath)) {
      return 'recycle';
    }

    //next try crop
    if (isset($values['arguments']['cropbox_x'], $values['arguments']['cropbox_y'], $values['arguments']['cropbox_width'], $values['arguments']['cropbox_height'])) {
      return 'crop';
    }

    //just keep the image as is
    return 'keep';
  }

  /**
   * @function _recycle
   *
   * Both original and thumbnail images are present.  Save them both, updated
   * the db, and return the fid
   */
  function _recycle($original_file, $gid, $uid) {
    $thumb_filepath = variable_get('os_migrate_files') . '/' . $original_file;
    $full_filepath = $thumb_filepath . '.crop_display.jpg';

    $full = new stdClass();
    $full->uid = $uid;
    $full->filemime = file_get_mimetype($full_filepath);
    $full->uri = $full_filepath;

    if ($full = file_copy($full, 'public://') && $gid) {
      $full_entity = entity_metadata_wrapper('file', $full);
      $full_entity->og_group_ref = array($gid);
      $full_entity->save();
    }

    $thumb = new stdClass();
    $thumb->uid = $uid;
    $thumb->filemime = file_get_mimetype($thumb_filepath);
    $thumb->uri = $thumb_filepath;

    if ($thumb = file_copy($thumb, 'public://') && $gid) {
      $thumb_entity = entity_metadata_wrapper('file', $thumb);
      $thumb_entity->og_group_ref = array($gid);
      $thumb_entity->save();
    }

    file_usage_add($full, 'imagefield_crop', 'file', $thumb->fid);
    return $thumb; //not sure about this.
  }

  /**
   * @function _save_original
   *
   *  image crop needs the file its cropping to exist as an entity before the crop can happen
   *  save the image here.
   *
   * @param string $original_file
   *   path to original image file
   * @param int gid
   *   vsite id.
   * @param int uid
   *   user id.
   * @return int $fid
   *   file id where $original file is saved
   */
  function _save_original($original_file, $gid, $uid){
    $full = $original_file . '.crop_display.jpg';
    $filepath = variable_get('os_migrate_files') . '/' . $original_file;

    if (!is_readable($filepath)) {
      return FALSE;
    }

    $file = new stdClass();
    $file->uid = $uid;
    $file->filemime = file_get_mimetype($filepath);
    $file->uri = $filepath;

    if ($file = file_copy($file, 'public://') && $gid) {
      // attach metadata and save
      $entity = entity_metadata_wrapper('file', $file);
      $entity->og_group_ref = array($gid);
      $entity->save();
      return $file;
    }

    return FALSE;
  }

  //we need cropping values.  if they aren't provided, use the entire image.
  function _image_dimensions(&$values) {
    if (!isset($values['arguments']['cropbox_x'])) {
      $values['arguments']['cropbox_x'] = 0;
    }
    if (!isset($values['arguments']['cropbox_y'])) {
      $values['arguments']['cropbox_y'] = 0;
    }

    $file = variable_get('os_migrate_files') . '/' . $values[0];
    if (($dim = getimagesize($file)) == FALSE) {
      $dim = array(140,180); //use defaults from preset.
    }

    if (!isset($values['arguments']['cropbox_width'])) {
      $values['arguments']['cropbox_width'] = $dim[0];
    }
    if (!isset($values['arguments']['cropbox_height'])) {
      $values['arguments']['cropbox_height'] = $dim[1];
    }

  }
}

