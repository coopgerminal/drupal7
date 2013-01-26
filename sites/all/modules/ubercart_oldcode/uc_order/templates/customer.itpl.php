<?php
// $Id: customer.itpl.php,v 1.5.2.2 2009/01/02 20:18:41 islandusurper Exp $

/**
 * This file is the default customer invoice template for Ubercart.
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
                  <b><?php echo t('Resum de la comanda:'); ?></b>
                </td>
              </tr>

              <tr>
                <td nowrap="nowrap">
                  <b><?php echo t('Setmana de recollida:'); ?></b>
                </td>
                <td width="98%">
                  <?php echo $_SESSION['nom_setmana']; ?>
                </td>
              </tr>

              <tr>
                <td colspan="2">

                  <table border="0" cellpadding="1" cellspacing="0" width="100%" style="font-family: verdana, arial, helvetica; font-size: small;">

                    <tr>
                      <td colspan="2">
                        <br /><br /><b><?php echo t('Productes demanats:'); ?>&nbsp;</b>

                        <table width="100%" style="font-family: verdana, arial, helvetica; font-size: small;">

                          <?php if (is_array($order->products)) {
                            foreach ($order->products as $product) { ?>
                          <tr>
                            <td valign="top" nowrap="nowrap">
                              <b><?php echo $product->qty; ?> &nbsp;x&nbsp; <?php echo $product->title; ?></b>
							</td>
                            <td align="right">
                              <b><?php echo uc_currency_format($product->price * $product->qty); ?></b>
                            </td>
                          </tr>
                          <?php }
                              }?>
                              
                          <tr>
                           <td nowrap="nowrap">
                              <b><?php echo t('Total comanda:'); ?>&nbsp;</b>
                           </td>
                           <td align="right">
                             <b>[order-subtotal]</b>
                           </td>
                         </tr>            
                                                              
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
</td>
</tr>
</table>
