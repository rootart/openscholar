<?php

use Drupal\DrupalExtension\Context\DrupalContext;
use Behat\Behat\Context\Step\Given;
use Behat\Gherkin\Node\TableNode;
use Guzzle\Service\Client;
use Behat\Behat\Context\Step;

require 'vendor/autoload.php';

class FeatureContext extends DrupalContext {

  /**
   * Variable to pass into the last xPath expression.
   */
  private $xpath = '';

  /**
   * The box delta we need to hide.
   */
  private $box = '';

  /**
   * Hold the user name and password for the selenium tests for log in.
   */
  private $drupal_users;

  /**
   * Hold the NID of the vsite.
   */
  private $nid;

  /**
   * Initializes context.
   *
   * Every scenario gets its own context object.
   *
   * @param array $parameters.
   *   Context parameters (set them up through behat.yml or behat.local.yml).
   */
  public function __construct(array $parameters) {
    if (isset($parameters['drupal_users'])) {
      $this->drupal_users = $parameters['drupal_users'];
    }

    if (isset($parameters['vsite'])) {
      $this->nid = $parameters['vsite'];
    }
  }

  /**
   * Authenticates a user with password from configuration.
   *
   * @Given /^I am logged in as "([^"]*)"$/
   */
  public function iAmLoggedInAs($username) {
    try {
      $password = $this->drupal_users[$username];
    } catch (Exception $e) {
      throw new Exception("Password not found for '$username'.");
    }

    // Log in.
    // Go to the user page.
    $element = $this->getSession()->getPage();
    $this->getSession()->visit($this->locatePath('/user'));
    $element->fillField('Username', $username);
    $element->fillField('Password', $password);
    $submit = $element->findButton('Log in');
    $submit->click();
  }

  /**
   * @Given /^I am on a "([^"]*)" page titled "([^"]*)"(?:, in the tab "([^"]*)"|)$/
   */
  public function iAmOnAPageTitled($page_type, $title, $subpage = NULL) {
    $table = 'node';
    $id = 'nid';
    $path = "$page_type/%";
    $type = str_replace('-', '_', $page_type);

    $path .= "/$subpage";

    //TODO: The title and type should be properly escaped.
    $query = "\"
      SELECT $id AS identifier
      FROM $table
      WHERE title = '$title'
      AND type = '$type'
    \"";

    $result = $this->getDriver()->drush('sql-query', array($query));
    $id = trim(substr($result, strlen('identifier')));

    if (!$id) {
      throw new \Exception("No $page_type with title '$title' was found.");
    }
    $path = str_replace('%', $id, $path);

    return new Given("I am at \"node/$id\"");
  }

  /**
   * @Given /^I should see the "([^"]*)" table with the following <contents>:$/
   */
  public function iShouldSeeTheTableWithTheFollowingContents($class, TableNode $table) {
    $page = $this->getSession()->getPage();
    $table_element = $page->find('css', "table.$class");
    if (!$table_element) {
      throw new Exception("A table with the class $class wasn't found");
    }

    $table_rows = $table->getRows();
    $hash = $table->getRows();
    // Iterate over each row, just so if there's an error we can supply
    // the row number, or empty values.
    foreach ($table_rows as $i => $table_row) {
      if (empty($table_row)) {
        continue;
      }
      if ($diff = array_diff($hash[$i], $table_row)) {
        throw new Exception(sprintf('The "%d" row values are wrong.', $i + 1));
      }
    }
  }

  /**
   * @When /^I clear the cache$/
   */
  public function iClearTheCache() {
    $this->getDriver()->drush('cc all');
  }


  /**
   * @Then /^I should print page$/
   */
  public function iShouldPrintPage() {
    $this->getSession()->visit($this->locatePath('/user'));
    $element = $this->getSession()->getPage();
    $element->fillField($this->getDrupalText('username_field'), 'admin');
    $element->fillField($this->getDrupalText('password_field'), 'admin');
    $submit = $element->findButton($this->getDrupalText('log_in'));
    if (empty($submit)) {
      throw new \Exception('No submit button at ' . $this->getSession()->getCurrentUrl());
    }

    // Log in.
    $submit->click();
    $element = $this->getSession()->getPage();
    print_r($element->getContent());
  }

  /**
   * @Then /^I should get:$/
   */
  public function iShouldGet(PyStringNode $string) {
    throw new PendingException();
  }

  /**
   * @Then /^I should see the images:$/
   */
  public function iShouldSeeTheImages(TableNode $table) {
    $page = $this->getSession()->getPage();
    $table_rows = $table->getRows();
    foreach ($table_rows as $rows) {
      $image = $page->find('xpath', "//img[contains(@src, '{$rows[0]}')]");
      if (!$image) {
        throw new Exception(sprintf('The image "%s" wasn\'t found in the page.', $rows[0]));
      }
    }
  }

  /**
   * @Given /^I drag&drop "([^"]*)" to "([^"]*)"$/
   */
  public function iDragDropTo($element, $destination) {
    $selenium = $this->getSession()->getDriver();
    $selenium->evaluateScript("jQuery('#{$element}').detach().prependTo('#{$destination}');");
  }

  /**
   * @Given /^I verify the element "([^"]*)" under "([^"]*)"$/
   */
  public function iVerifyTheElementUnder($element, $container) {
    $page = $this->getSession()->getPage();
    $element = $page->find('xpath', "//*[contains(@id, '{$container}')]//div[contains(@id, '{$element}')]");

    if (!$element) {
      throw new Exception(sprintf("The element with %s wasn't found in %s", $element, $container));
    }
  }

  /**
   * Find any element that contain the text and has the css class or id
   * selector.
   */
  private function findAnyElement($text, $container, $childElement = '*') {
    $page = $this->getSession()->getPage();
    $attributes = array(
      'id',
      'class',
    );

    // Find the element wrapped under an element with the class.
    foreach ($attributes as $attribute) {
      $this->xpath = "//*[contains(@$attribute, '{$container}')]/{$childElement}[contains(., '{$text}')]";
      $element = $page->find('xpath', $this->xpath);
      if ($element) {
        return $element;
      }
    }

    // Find the element with the class.
    foreach ($attributes as $attribute) {
      $this->xpath = "//*[contains(@$attribute, '{$container}') and contains(., '{$text}')]";
      $element = $page->find('xpath', $this->xpath);
      if ($element) {
        return $element;
      }
    }

    throw new Exception(sprintf("An element containing the text %s with the class %s wasn't found", $text, $container));
  }

  /**
   * @Given /^I create a new publication$/
   */
  public function iCreateANewPublication() {
    return array(
      new Step\When('I visit "john/node/add/biblio"'),
      new Step\When('I select "Book" from "Publication Type"'),
      new Step\When('I press "edit-biblio-next"'),
      new Step\When('I fill in "Title" with "'. time() . '"'),
      new Step\When('I press "edit-submit"'),
    );
  }

  /**
   * @Given /^the widget "([^"]*)" is set in the "([^"]*)" page$/
   */
  public function theWidgetIsSetInThePage($page, $widget) {
    $code = "os_migrate_demo_set_box_in_region({$this->nid}, '$page', '$widget');";
    $this->box[] = $this->getDriver()->drush("php-eval \"{$code}\"");
  }

  /**
   * Hide the boxes we added during the scenario.
   *
   * @AfterScenario
   */
  public function afterScenario($event) {
    if (empty($this->box)) {
      return;
    }

    // Loop over the box we collected in the scenario, hide them and delete
    // them.
    foreach ($this->box as $box_handler) {
      $data = explode(',', $box_handler);
      $code = "os_migrate_demo_hide_box({$this->nid}, '{$data[0]}', '{$data[1]}', '{$data[2]}');";
      $this->getDriver()->drush("php-eval \"{$code}\"");
    }
  }

  /**
   * @Given /^I should see the following <links>$/
   */
  public function iShouldNotSeeTheFollowingLinks(TableNode $table) {
    $page = $this->getSession()->getPage();
    $hash = $table->getRows();

    foreach ($hash as $i => $table_row) {
      if (empty($table_row)) {
        continue;
      }
      $element = $page->find('xpath', "//a[.='{$table_row[0]}']");

      if (empty($element)) {
        throw new Exception(printf("The link %s wasn't found on the page", $table_row[0]));
      }
    }
  }
}
