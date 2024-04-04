<?php

namespace Drupal\front;

use Drupal\Core\PathProcessor\OutboundPathProcessorInterface;
use Drupal\Core\Render\BubbleableMetadata;
use Symfony\Component\HttpFoundation\Request;

/**
 * Class FrontPagePathProcessor.
 *
 * @package Drupal\front
 */
class FrontPagePathProcessor implements OutboundPathProcessorInterface {

  /**
   * {@inheritdoc}
   */
  public function processOutbound($path, &$options = [], Request $request = NULL, BubbleableMetadata $bubbleable_metadata = NULL) {
    if ($path == '/main') {
      $path = '';
    }

    $config = \Drupal::config('front.settings');
    $new_path = $config->get('home_link_path', '');
    if (($path === '/<front>' || empty($path)) && !empty($new_path)) {
      $path = '/' . $new_path;
    }
    return $path;
  }

}
