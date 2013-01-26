<div class="art-Block clear-block block block-<?php print $block->module ?>" id="block-<?php print $block->module .'-'. $block->delta; ?>">
    <div class="art-Block-tl art-Block-tl-2"></div>
    <div class="art-Block-tr art-Block-tr-2"></div>
    <div class="art-Block-bl art-Block-bl-2"></div>
    <div class="art-Block-br art-Block-br-2"></div>
    <div class="art-Block-tc art-Block-tc-2"></div>
    <div class="art-Block-bc art-Block-bc-2"></div>
    <div class="art-Block-cl art-Block-cl-2"></div>
    <div class="art-Block-cr art-Block-cr-2"></div>
    <div class="art-Block-cc art-Block-cc-2"></div>
    <div class="art-Block-body">

	<?php if ($block->subject): ?>
<div class="art-BlockHeader">
		    <div class="germ-blk-tag-icon">
		        <div class="t">	
			<h2 class="subject GERMTEXTBLANC"><?php echo $block->subject; ?></h2>
</div>
		    </div>
		</div>    
	<?php endif; ?>
<div class="art-BlockContent content">
	    <div class="art-BlockContent-body GERMTEXTBLANC" >
	
		<?php echo $block->content; ?>

	    </div>
	</div>
	

    </div>
</div>
