<?php

/**
 * @file
 * Drush file for Islandora Groovy.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_groovy_drush_command() {
  $items = array();

  $items['generate_images'] = array(
    'aliases' => array('igroovy'),
    'description' => 'Generates some groovy images and loads them into the Islandora Basic Image Collection.',
    'drupal dependencies' => array('islandora_batch', 'islandora_basic_image'),
    'examples' => array(
      'drush igroovy',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );

  return $items;
}

function drush_islandora_groovy_generate_images() {
  $output_dir = drupal_realpath('public://') . DIRECTORY_SEPARATOR . 'groovy';
  if (!file_exists($output_dir)) {
    drupal_mkdir($output_dir);
  }

  $feelings = array(
    'peace' => array('color' => 'pink', 'desc' => 'Peace man.'),
    'love' => array('color' => 'red', 'desc' => 'Love everybody.'),
    'kindness' => array('color' => 'blue', 'desc' => 'Karma baby.'),
    'harmony' => array('color' => 'green', 'desc' => 'Nature is groovy.'),
    'warmth' => array('color' => 'purple', 'desc' => 'Feel the sunshine.'),
    'energy' => array('color' => 'orange', 'desc' => 'Let the energy flow through you.'),
  );

  foreach ($feelings as $feeling => $details) {
    // Generate some images with ImageMagick.
    $cmd = "convert -size 1000x1000 xc:" . $details['color'] . " " . $output_dir .
      DIRECTORY_SEPARATOR . $feeling . ".jpg;  convert -size 1000x1000 xc:" .
      $details['color'] . " -pointsize 200 -fill white -gravity center -annotate +0+0 '" .
      $feeling . "' " . $output_dir . DIRECTORY_SEPARATOR . $feeling . ".jpg";
    drush_print("Generating OBJ file for $feeling ...");
    exec($cmd);

    // Generate corresponding MODS files.
    drush_print("Generating MODS file for $feeling ...");
    $mods = '<?xml version="1.0"?>';
    $mods .= '<mods xmlns="http://www.loc.gov/mods/v3" ';
    $mods .= 'xmlns:mods="http://www.loc.gov/mods/v3" ';
    $mods .= 'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" ';
    $mods .= 'xmlns:xlink="http://www.w3.org/1999/xlink">';
    $mods .= '<titleInfo><title>' . $feeling .'</title></titleInfo>';
    $mods .= '<originInfo><dateIssued encoding="iso8601" keyDate="yes">1966</dateIssued></originInfo>';
    $mods .= '<typeOfResource>still image</typeOfResource>';
    $mods .= '<abstract>' . $details['desc'] . '</abstract>';
    $mods .= '</mods>';
    $mods_path = $output_dir .  DIRECTORY_SEPARATOR . $feeling . '.xml';
    file_put_contents($mods_path, $mods);
  }

  drush_print("Ingesting objects and creating derivatives. Stay cool, this can take a minute."); 

  // Finally, load 'em into Islandora.
  $batch_cmd = 'drush -u 1 --uri=http://localhost islandora_batch_scan_preprocess';
  $batch_cmd .= ' --namespace=islandora';
  $batch_cmd .= ' --content_models=islandora:sp_basic_image --parent=islandora:sp_basic_image_collection';
  $batch_cmd .= ' --parent_relationship_pred=isMemberOfCollection --type=directory --target=' . $output_dir;
  exec($batch_cmd);
  $ingest_cmd = 'drush -u 1 --uri=http://localhost islandora_batch_ingest';
  exec($ingest_cmd);

  drush_print("Your groovy images have been loaded. Peace out."); 
}
