<?php

/**
 * @file os_migrate_node_biblio.inc
 *
 */

class BiblioNodeOSMigration extends AbstractNodeOSMigration {
  public function __construct() {
    /**
     * AbstractNodeOSMigration's __construct() will set up most of the common node migration.
     */
    parent::__construct('biblio', 'biblio'); //args are d7 bundle, d6 content type.

    $this->description = t('Publications migration.');
    $this->dependencies[] = 'BiblioContributorTypeTable';
    $this->dependencies[] = 'BiblioContributorTypeDataTable';


    //$this->addUnmigratedDestinations($fields)
    $this->addUnmigratedDestinations(parent::subfields('field_biblio_image', array('file_class', 'language', 'destination_dir', 'destination_file', 'file_replace', 'preserve_files', 'source_dir')), t('DNM'));

    $this->addUnmigratedSources(array('field_biblio_image_fid', 'field_biblio_image_list', 'field_biblio_image_data'), t('DNM'));

    $this->addSimpleMappings(array('field_biblio_image', 'field_biblio_image:alt', 'field_biblio_image:title'));


  }

  public function query($content_type) {
    $query = parent::query($content_type);

    /**
     * Modify the query.  Add joins and fields here.  content_type_${content_type} and its fields are already included.
     */

    //biblio table
    $query->join('biblio', 'b', 'n.vid = b.vid');
    $query->fields('b');

    return $query;
  }


  public function sourceFields($content_type) {
    $fields = parent::sourceFields($content_type);

    /**
     * Describe each of the source columns
     */
    $fields += array(
      'field_biblio_image' => t('Book cover'),
    	'field_biblio_image:alt' => t('Book cover alt text'),
    	'field_biblio_image:title' => t('Book cover title'),
    );

    return $fields;
  }

  public function prepareRow($current_row) {

    /**
     * This method is called while looping over the results of the query provided earlier.
     *
     * This is your chance to munge data.  (See Abstract's filter formats).
     *
     * This is also your chance to query additional tables that didn't make sense to join earlier
     * (for example, see gallery.  d6's gallery had references to image nodes.  d7's gallery uses
     * file fields for images.  While looping over d6 gallery nodes, prepareRow uses additional queries
     * to get those image nodes and extract everything needed by file fields.)
     */

		if ($current_row->field_biblio_image_fid) {
		  parent::d6_db_select('files', 'f')
		  	->condition('f.fid', $current_row->field_biblio_image_fid, '=')
		  	->fields('f');

		  if ($current_row->field_biblio_image_data && $data = unserialize($current_row->field_biblio_image_data)) {
		    $current_row->{'field_biblio_image:alt'} = isset($data['alt']) ? $data['alt'] : '';
		    $current_row->{'field_biblio_image:title'} = isset($data['title']) ? $data['title'] : '';
		  }
		}

		return parent::prepareRow($current_row); //false -> skip this row


  }

  /**
   * @function prepare()
   *
   * Biblio's fields aren't really fields so migrate isn't aware of them.  But migrate provides
   * us with the prepare function, which lets you modify an entity before it's saved.
   */
  function prepare($entity, $row) {
    $ignore = array('biblio_md5');
    foreach(get_object_vars($row) as $key => $value) {
      if (strpos($key, 'biblio_') === 0 && $row->$key && !in_array($key, $ignore)) {
        $entity->$key = $value;
      }
    }

    $entity->biblio_contributors = $this->_prepare_biblio_contributors($row->vid, $row->nid);
    // $entity->biblio_keywords = $this->_prepare_biblio_keywords($row->vid); //not supporting keywords.
    //$entity->vid = $row->vid;
  }

  /**
   * @see Migration::preRollback()
   *
   * Make a list of migrated nodes before rollback.  Post rollback will use this
   * to determine which rows to delete from duplicates.  This isn't necessary for
   * just migrating, but for a migration mixed with new nodes, we don't want to
   * delete any new nodes just because they're not in the migrate map.
   */
  function preRollback() {
    $query = db_query('SELECT destid1 FROM migrate_map_biblionodeos');
    $nids = $query->fetchCol('destid1');
    variable_set('os_migrate_biblio_prerollback', serialize($nids));
  }

  /**
   * @see Migration::postRollback()
   *
   * Biblio does not delete from the duplicates table after a node is deleted.
   * This means that duplicates that are rolled back will have a collision the
   * next time they're imported.
   *
   * Instead, clear out after rolling back.
   */
  function postRollback() {
    $query = db_query('SELECT destid1 FROM migrate_map_biblionodeos');
    $remaining_nids = $query->fetchCol('destid1');

    $pre_nids = variable_get('os_migrate_biblio_prerollback', FALSE);
    variable_del('os_migrate_biblio_prerollback');

    if ($pre_nids) {
      $pre_nids = unserialize($pre_nids);

      $deleted_nids = array_diff($pre_nids, $remaining_nids);
      if (count($deleted_nids)) {
        db_delete('biblio_duplicates')
          ->condition(db_or()->condition('vid', $deleted_nids, 'IN')->condition('did', $deleted_nids, 'IN'))
          ->execute();


      }
    }
  }

  /**
   * @function _prepare_biblio_contributors
   *
   * Builds the contributors array from the d6 table.
   *
   * @param int $vid
   *   Revision of biblio node with contributors.
   * @return array()
   *   Returns array of contributor arrays to attach to node
   */
  function _prepare_biblio_contributors($vid, $nid) {
    $contributors = array();
    $query = parent::d6_db_select('biblio_contributor', 'bc')
      ->fields('bc', array('nid', 'vid', 'auth_type', 'auth_category', 'rank'))
      ->condition('bc.vid', $vid, '=');
    $query->join('biblio_contributor_data', 'bc_data', 'bc.cid = bc_data.cid');
    $query->fields('bc_data', array('name', 'lastname'));
    $result = $query->execute();

    while ($r = $result->fetchAssoc()) {
      //in d6 we used secondary authors as editors.  new os_publications uses all sorts of roles, but we should convert secondary
      if ($r['auth_type'] == 2) {
        $r['auth_type'] = 14;
      }
      $r['cid'] = NULL;
      $contributors[] = $r;
    }

    return $contributors;
  }

  //gets keywords.  returns array of id => word.
  function _prepare_biblio_keywords($vid) {
    $keywords = array();
    //lets try just using bib nodes with keywords
    $query = parent::d6_db_select('biblio_keyword', 'bk')
      ->condition('bk.vid', $vid, '=');

    $query->join('biblio_keyword_data', 'bkd', 'bk.kid = bkd.kid');
    $query->fields('bkd', array('kid', 'word'));
    $results = $query->execute();

    while ($r = $results->fetchAssoc()) {
      $keywords[ $r['kid'] ] = $r['word'];
    }
    //biblio_get_keyword_by_id($kid) //might be useful?
    return $keywords;
  }

}