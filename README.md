Front Page
==========

This module allows you to specify a custom front page based on the user's role. 


Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules


Help and Functionality
----------------------

Some notes about this module:

* Allows you to specify a custom front page based on role type.
* Allows 4 different override types:
  * Themed - Allows you to add content that will display as a standard
    	     themed Drupal page.
  * Full - Allows you to add content that will be displayed on the screen
             as is. This method is the same as declaring a whole HTML page.
			 Be aware that for this type, you need to use a text format with
			 no HTML filters applied, or it may strip out styles, <head> tags, etc.
  * Redirect - Allows you to 301 redirect the user to another path.
  * Alias - Allows you to specify a local path which will then display as
          the home page without redirecting the user.
* Allow Themed and Full display types to be passed through Drupals input filters.
* Override Home Links to go to another local path. This could be to stop users
    going back to a splash screen.



Current Maintainers
-------------------

- [Richard Peacock](https://github.com/swampopus) - Originally ported to Backdrop CMS.
- Seeking additional maintainers.

Credits
-------

This module is based on the Drupal module front-7.x-2.4

Project page: https://www.drupal.org/project/front

Drupal Maintainers:
- [Julian Pustkuchen](https://www.drupal.org/u/anybody)
- [Gus](https://www.drupal.org/u/dublin-drupaller)
- [Joshua Sedler](https://www.drupal.org/u/grevil)
- [Samuel Solís](https://www.drupal.org/u/estoyausente)
- [Oleksandr Senenko](https://www.drupal.org/u/oleksandr-senenko)
- [Simon Georges](https://www.drupal.org/u/simon-georges)



License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.