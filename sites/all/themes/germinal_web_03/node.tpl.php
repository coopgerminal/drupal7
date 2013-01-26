<div class="art-Post">
    <div class="art-Post-body">
<div class="art-Post-inner">
<?php ob_start(); ?>
<h2 class="art-PostHeader"> <a href="<?php echo $node_url; ?>" title="<?php echo $title; ?>"><?php echo $title; ?></a>
</h2>
<?php $metadataContent = ob_get_clean(); ?>
<?php if (trim($metadataContent) != ''): ?>
<div class="art-PostMetadataHeader">
<?php echo $metadataContent; ?>

</div>
<?php endif; ?>
<?php if ($submitted): ?>
<div class="art-PostHeaderIcons art-metadata-icons">
<?php echo art_submitted_worker($date, $name); ?>

</div>
<?php endif; ?>
<div class="art-PostContent">
<div class="art-article"><?php print $picture; ?><?php echo $content; ?>
<?php if (isset($node->links['node_read_more'])) { echo '<div class="read_more">'.get_html_link_output($node->links['node_read_more']).'</div>'; }?></div>
</div>
<div class="cleared"></div>
<?php if (is_art_links_set($node->links) || !empty($terms)):
$output = art_node_worker($node); 
if (!empty($output)):	?>
<div class="art-PostFooterIcons art-metadata-icons">
<?php echo $output; ?>

</div>
<?php endif; endif; ?>

</div>

    </div>
</div>
