Blockly.Blocks["plantower_pms7003.getpm2_5"] = {
	init: function() {
		this.appendDummyInput()
			.appendField(Blockly.Msg.plantower_pms7003_GET_PM2_5_TEXT_TITLE);

		this.setOutput(true, 'Number');
		this.setInputsInline(true);
		this.setPreviousStatement(false);
		this.setNextStatement(false);
		this.setColour(58);
		this.setTooltip(Blockly.Msg.plantower_pms7003_GET_PM2_5_TEXT_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.plantower_pms7003_GET_PM2_5_TEXT_HELPURL);
	}
};




Blockly.Blocks["plantower_pms7003.getpm10"] = {
	init: function() {
		this.appendDummyInput()
			.appendField(Blockly.Msg.plantower_pms7003_GET_PM10_TEXT_TITLE);

		this.setOutput(true, 'Number');
		this.setInputsInline(true);
		this.setPreviousStatement(false);
		this.setNextStatement(false);
		this.setColour(58);
		this.setTooltip(Blockly.Msg.plantower_pms7003_GET_PM10_TEXT_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.plantower_pms7003_GET_PM10_TEXT_HELPURL);
	}
};
