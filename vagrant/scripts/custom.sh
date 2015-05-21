# If you're using the Islandora Vagrant (https://github.com/Islandora-Labs/islandora_vagrant)
# and you want to have Groovy load the images for you, add this script to the scripts directory
# in your Islandora Vagrant instance before running 'vagrant up'.

echo "Installing some sample content using Islandora Groovy"

cd /var/www/drupal/sites/all/modules
git clone https://github.com/mjordan/islandora_groovy.git
drush --yes en islandora_groovy
drush --yes cc drush
drush igroovy
