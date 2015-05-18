<?php

/**
 * @file
 * Drush hooks.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_groovy_drush_command() {
  $items = array();

  $items['generate_images'] = array(
    'aliases' => array('igroovy'),
    'description' => 'Generates Islandora Groovy images.',
    'drupal dependencies' => array('islandora_batch'),
    'examples' => array(
      'drush -v igroovy',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );

  return $items;
}

function drush_islandora_groovy_generate_images() {
  $output_dir = drupal_realpath('public://') . '/groovy';
  if (!file_exists($output_dir)) {
    drupal_mkdir($output_dir);
  }

  $combinations = array(
    'peace' => array('color' => 'pink', 'desc' => 'Peace man.'),
    'love' => array('color' => 'red', 'desc' => 'Love everybody.'),
    'kindness' => array('color' => 'blue', 'desc' => 'Karma baby.'),
    'harmony' => array('color' => 'green', 'desc' => 'Nature is groovy.'),
    'warmth' => array('color' => 'purple', 'desc' => 'Feel the sunshine.'),
    'energy' => array('color' => 'black', 'desc' => 'The universe is energy.'),
  );

  foreach ($combinations as $feeling => $details) {
    $cmd = "convert -size 1000x1000 xc:" . $details['color'] . " " . $output_dir .
      DIRECTORY_SEPARATOR . $feeling . ".jpg;  convert -size 1000x1000 xc:" .
      $details['color'] . " -pointsize 200 -fill white -gravity center -annotate +0+0 '" .
      $feeling . "' " . $output_dir . DIRECTORY_SEPARATOR . $feeling . ".jpg";
    drush_print("Generating $feeling image...");
    exec($cmd);

    drush_print("Generating $feeling MODS...");
    $mods = '<?xml version="1.0"?>' . "\n";
    $mods .= '<mods xmlns="http://www.loc.gov/mods/v3" xmlns:mods="http://www.loc.gov/mods/v3" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xlink="http://www.w3.org/1999/xlink">' ."\n";
    $mods .= "<titleInfo>\n<title>" . $feeling ."</title\n></titleInfo>";
    $mods .= '<originInfo><dateIssued encoding="iso8601" keyDate="yes">1966</dateIssued> </originInfo>' . "\n";
    $mods .= '<typeOfResource>still image</typeOfResource>' . "\n";
    $mods .= "<abstract>" . $details['desc'] . "</abstract>\n";
    $mods .= '</mods>';
    $mods_path = $output_dir .  DIRECTORY_SEPARATOR . $feeling . '.xml';
    file_put_contents($mods_path, $mods);
  }

  $batch_cmd = 'drush -v -u 1 --uri=http://localhost islandora_batch_scan_preprocess' .
  $batch_cmd .= ' --namespace=islandora';
  $batch_cmd .= ' --content_models=islandora:sp_basic_image --parent=islandora:sp_basic_image_collection' .
  $batch_cmd .= ' --parent_relationship_pred=isMemberOfCollection --type=directory --target=' . $output_dir;
  exec($batch_cmd);
  $ingest_cmd = 'drush -v -u 1 --uri=http://localhost islandora_batch_ingest';
  exec($ingest_cmd);
}
