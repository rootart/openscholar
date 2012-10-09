<?php
/**
 * @file
 * Adaptivetheme implementation to display a single Drupal page.
 *
 * Available variables:
 *
 * Adaptivetheme supplied variables:
 * - $linked_site_logo: Themed logo.
 * - $hide_site_name: toggle_name boolean.
 * - $visibility: Holds the class .element-invisible or is empty.
 * - $primary_navigation: Themed Main menu.
 * - $secondary_navigation: Themed Secondary/user menu.
 * - $primary_local_tasks: Split local tasks - primary.
 * - $secondary_local_tasks: Split local tasks - secondary.
 * - $tag: Generates the wrapper element for the main content.
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/bartik.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or
 *   prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $site_name: The name of the site, empty when display has been disabled
 *   in theme settings.
 * - $site_slogan: The slogan of the site, empty when display has been disabled
 *   in theme settings.
 *
 * Navigation:
 * - $main_menu (array): An array containing the Main menu links for the
 *   site, if they have been configured.
 * - $secondary_menu (array): An array containing the Secondary menu links for
 *   the site, if they have been configured.
 * - $breadcrumb: The breadcrumb trail for the current page.
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title: The page title, for use in the actual HTML content.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 * - $messages: HTML for status and error messages. Should be displayed
 *   prominently.
 * - $tabs (array): Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links (array): Actions local to the page, such as 'Add menu' on the
 *   menu administration interface.
 * - $feed_icons: A string of all feed icons for the current page.
 * - $node: The node object, if there is an automatically-loaded node
 *   associated with the page, and the node ID is the second argument
 *   in the page's path (e.g. node/12345 and node/12345/revisions, but not
 *   comment/reply/12345).
 *
 * Regions:
 * - $page['help']: Dynamic help text, mostly for admin pages.
 * - $page['highlighted']: Items for the highlighted content region.
 * - $page['content']: The main content of the current page.
 * - $page['sidebar_first']: Items for the first sidebar.
 * - $page['sidebar_second']: Items for the second sidebar.
 * - $page['header']: Items for the header region.
 * - $page['footer']: Items for the footer region.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see template_process()
 * @see adaptivetheme_preprocess_page()
 * @see adaptivetheme_process_page()
 */
?>
 <!--FLEXIBLE ADMIN HEADER FOR USE BY SELECT GROUPS USING OS-->
<?php if ($page['branding_header']): ?>
<div id="branding_header">
	<?php print render($page['branding_header']); ?>
</div>	
<?php endif; ?>	

<div id="page" class="container <?php print $classes; ?>">
	<div id="page-wrapper">
<!--header regions beg-->
<?php if (
  $page['header_top'] ||
  $page['header_first'] ||
  $page['header_second'] ||
  $page['header_third'] ||
  $page['header_bottom']
  ): ?>
 <header id="header" class="clearfix" role="banner">
  <div id="header-container">
  <div class="at-panel gpanel panel-display three-col clearfix">
    <?php print render($page['header_top']); ?>
    <?php print render($page['header_second']); ?>
    <?php print render($page['header_first']); ?>
    <?php print render($page['header_third']); ?>
    <?php print render($page['header_bottom']); ?>
  </div>
  </div>
  </header>
<?php endif; ?>
<!--header regions end-->
<!--main menu region beg-->
  <?php print render($page['menu_bar']); ?>
  <!--main menu region end-->

<?php print $messages; ?>
  <?php print render($page['help']); ?>

<div id="columns" class="clearfix">
	<div class="hg-container">
    <div id="content-column" role="main">
      <div class="content-inner">
      	<?php if ($is_front): ?>
          <?php print render($title_suffix); ?>
<?php if (
  $page['content_top'] ||
  $page['content_first'] ||
  $page['content_second'] ||
  $page['content_bottom']
  ): ?>
  <div class="at-panel gpanel panel-display content clearfix">
    <?php print render($page['content_top']); ?>
    <?php print render($page['content_first']); ?>
    <?php print render($page['content_second']); ?>
    <?php print render($page['content_bottom']); ?>
  </div>
<?php endif; ?>
  <?php endif; ?>


   	<?php if (!$is_front): ?>
      	 <<?php print $tag; ?> id="main-content">

          <?php print render($title_prefix); ?>
          <?php if ($title || $primary_local_tasks || $secondary_local_tasks || $action_links = render($action_links)): ?>
            <header id="main-content-header">

              <?php if (!$is_front && $title): ?>
                <h1 id="page-title"<?php print $attributes; ?>>
                  <?php print $title; ?>
                </h1>
              <?php endif; ?>

              <?php if ($primary_local_tasks || $secondary_local_tasks || $action_links): ?>
                <div id="tasks">
                  <?php if ($primary_local_tasks): ?>
                    <ul class="tabs primary clearfix"><?php print render($primary_local_tasks); ?></ul>
                  <?php endif; ?>
                  <?php if ($secondary_local_tasks): ?>
                    <ul class="tabs secondary clearfix"><?php print render($secondary_local_tasks); ?></ul>
                  <?php endif; ?>
                  <?php if ($action_links = render($action_links)): ?>
                    <ul class="action-links clearfix"><?php print $action_links; ?></ul>
                  <?php endif; ?>
                </div>
              <?php endif; ?>

            </header>
          <?php endif; ?>
          <?php print render($title_suffix); ?>

          <?php if ($content = render($page['content'])): ?>
            <div id="content">
              <?php print $content; ?>
            </div>
          <?php endif; ?>

        </<?php print $tag; ?>><!--main content ends-->
        <?php endif; ?>
      </div>
       </div>
<!--sidebar first region beg-->
    <?php $sidebar_first = render($page['sidebar_first']); print $sidebar_first; ?>
<!--sidebar first region end-->
<!--sidebar second region beg-->
    <?php $sidebar_second = render($page['sidebar_second']); print $sidebar_second; ?>
<!--sidebar second region end-->
  </div>
 </div>

  <?php if ($page['footer']): ?>
    <footer id="footer" class="clearfix" role="contentinfo"><?php print render($page['footer']); ?></footer>
  <?php endif; ?>
  <?php if ($rolling_activity): ?>
    <div class='current-activity'>
      <span class='title'><?php print $rolling_activity['title']; ?></span>
      <div class='activity'>
        <?php print $rolling_activity['output']; ?>
      </div>
    </div>
  <?php endif; ?>
</div></div><!--page area ends-->
 <div id="extradiv"></div>
 
 <!--FLEXIBLE ADMIN FOOTER FOR USE BY SELECT GROUPS USING OS-->
 <?php if ($page['branding_footer']): ?>
<div id="branding_footer">
	<?php print render($page['branding_footer']); ?>
</div>	
<?php endif; ?>	
