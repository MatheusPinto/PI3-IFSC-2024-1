/* Arquivo do node_helper.js */
const NodeHelper = require("node_helper");

const fs = require("fs");

module.exports = NodeHelper.create({
  // Inicia o node_helper
  start: function () {
    console.log("MagicLearning helper started...");
  }
});

