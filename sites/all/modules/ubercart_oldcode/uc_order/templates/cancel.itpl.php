<?php
// $Id: customer.itpl.php,v 1.5.2.2 2009/01/02 20:18:41 islandusurper Exp $

/**
 * Template del mail de cancel·lació de comanda.
 */
?>

<table width="95%" border="0" cellspacing="0" cellpadding="1" align="center" bgcolor="#006699" style="font-family: verdana, arial, helvetica; font-size: small;">
  <tr>
    <td>
      <table width="100%" border="0" cellspacing="0" cellpadding="5" align="center" bgcolor="#FFFFFF" style="font-family: verdana, arial, helvetica; font-size: small;">
        <?php if ($business_header) { ?>
        <tr valign="top">
          <td>
            <table width="100%" style="font-family: verdana, arial, helvetica; font-size: small;">
              <tr>
                <td>
                  [site-logo]
                </td>
                <td width="98%">
                  <div style="padding-left: 1em;">
                  <span style="font-size: large;">[store-name]</span><br/>
                  [site-slogan]
                  </div>
                </td>
              </tr>
            </table>
          </td>
        </tr>
        <?php } ?>

        <tr valign="top">
          <td>

            <table cellpadding="4" cellspacing="0" border="0" width="100%" style="font-family: verdana, arial, helvetica; font-size: small;">
              <tr>
                <td colspan="2" bgcolor="#006699">
                  <b><?php echo t('Informació del soci:'); ?></b>
                </td>
              </tr>
              <tr>
                <td nowrap="nowrap">
                  <b><?php echo t('Número de soci:'); ?></b>
                </td>
                <td width="98%">
                  <?php echo $_SESSION['soci']->numsoci; ?>
                </td>
              </tr>
              <tr>
                <td nowrap="nowrap">
                  <b><?php echo t('Centre d\'activitat:'); ?></b>
                </td>
                <td width="98%">
                  <?php echo $_SESSION['centre_activitat']->name; ?>
                </td>
              </tr>

              <tr>
                <td colspan="2" bgcolor="#006699">
                  <b><?php echo t('Notificació:'); ?></b>
                </td>
              </tr>

              <tr>
                <td colspan="2">
                  <b><?php echo t('La seva comanda amb data de recollida %setmana ha estat anul&#183;lada.', array('%setmana' => $nom_setmana)); ?></b>
                </td>
              </tr>

            </table>
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>
</td>
</tr>
</table>
