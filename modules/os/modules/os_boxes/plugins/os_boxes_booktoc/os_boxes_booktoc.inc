<?php

/**
 * @file
 * The class for "Book Table of Contents" widgets.
 */
class os_boxes_booktoc extends os_boxes_default {
  /**
   * Implementation of boxes_content::options_defaults().
   */
  public function options_defaults() {
    $options = parent::options_defaults();

    $options['book_nid'] = 0;
    return $options;
  }

  /**
   * Implementation of boxes_content::options_form().
   */
  public function options_form(&$form_state) {

    // custom query
    $q = db_select('book', 'b');
    $q->join('node', 'n', 'b.bid = n.nid');
    $q->fields('b', array('bid'))
      ->fields('n', array('title'))
      ->distinct();

    if (module_exists('vsite') && $vsite = spaces_get_space()) {
      $q->join('og_membership', 'ogm', 'n.nid = ogm.etid');
      $q->condition('gid', $vsite->group->nid);
    }

    $results = $q->execute();
    $options = array(-1 => 'Current Book');
    foreach ($results as $row) {
      $options[$row->bid] = $row->title;
    }
    $form['book_nid'] = array(
      '#type' => 'select',
      '#title' => t('Which Book'),
      '#options' => $options,
      '#default_value' => $this->options['book_nid'],
    );

    $form += parent::options_form($form_state);
    return $form;
  }

  /**
   * Implementation of boxes_content::render().
   */
  public function render() {
    $block = parent::render();

    $book_nid = $this->options['book_nid'];
    if ($book_nid < 0) {
      // the toc of the book on the current page
      $node = menu_get_object();
  	  if(isset($node->book['bid']) && book_type_is_allowed($node->type)) {
  	    $book_nid = $node->book['bid'];
  	  }
  	  else {
  	    $block['title'] = $block['subject'] = '';
  	    return $block;
  	  }
    }

    $block = os_books_build_toc($book_nid) + $block;

    return $block;
  }

  /**
   * Return the cache parameters for this box
   */
  public function cache_info(){

    //Clear when book,class,class_material nodes are saved
    $cache_key = array(
      'os_og' => array('current' => 'current'),
      'node' => array('book' => 'book', 'class' => 'class', 'class_material' => 'class_material'),
    );

    return array('type' => BLOCK_CACHE_CONTENT_CACHE, 'keys' => $cache_key);
  }
}

/**
 * @file
 * A subclass for "Book Table of Contents" that shows the TOC for the current book
 *
class os_boxes_curr_toc extends os_boxes_booktoc {

	/**
   * Box instances of this type should not be avalible sitewide
   *
  public $sitewide = false;

	/**
   * Implementation of boxes_content::options_form().
   *
  public function options_form() {
  	$form = parent::options_form();
  	//Not needed for 'current book'
  	unset($form['book_nid']);
  	return $form;
  }

  /**
   * Implementation of boxes_content::render().
   *
  public function render() {
  	//Book TOC should not be displayed when it is in content
    if(scholar_book_toc_location() == SCHOLAR_BOOK_TOC_CONTENT) return array();

  	//Get the current book
  	$block = array();
  	$node = menu_get_object();
  	if(isset($node->book['bid']) && book_type_is_allowed($node->type)){
      $node = node_load($node->book['bid']);

      if ($node) {
        $this->options['book_nid'] = $node->nid;
        $block = parent::render();
      }
  	}

  	return $block;
  }

  /**
   * Return the cache parameters for this box
   *
  public function cache_info(){

    //Current TOC changes from page to page so don't cache
    return array();
  }

  /**
   * Do not show the box if they have TOC configured to show in the body of nodes
   *
  public function access(){
  	return !(scholar_book_toc_location() == SCHOLAR_BOOK_TOC_CONTENT);
  }

}
*/