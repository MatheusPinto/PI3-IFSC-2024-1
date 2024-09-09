Module.register("MagicLearning", {
  // Default module config.
  defaults: {
    updateInterval: 10 * 60 * 1000, // Atualizar a cada 10 minutos
    animationSpeed: 1000, // Velocidade da animação
    horariosFile: "horarios.json", // Arquivo com os horários dos ônibus
    linha: "linhaA" // Linha de ônibus padrão
  },

  // Define os estilos CSS a serem carregados
  getStyles: function () {
    return ["MagicLearning.css"];
  },

  // Inicializa o módulo
  start: function () {
    var self = this;
    // Inicia o temporizador para atualizar os horários
    setInterval(function () {
      self.updateHorarios();
    }, this.config.updateInterval);
    // Inicializa os horários
    this.horarios = null;
    this.readHorarios();
  },

  // Lê os horários do arquivo JSON
  readHorarios: function () {
    var self = this;
    var caminhoArquivo = this.file(this.config.horariosFile);
    this.sendSocketNotification("READ_HORARIOS", caminhoArquivo);
  },

  // Atualiza os horários
  updateHorarios: function () {
    if (!this.horarios) {
      this.readHorarios();
    } else {
      this.updateDom(this.config.animationSpeed);
    }
  },

  // Subclass socketNotificationReceived method.
  socketNotificationReceived: function (notification, payload) {
    if (notification === "HORARIOS") {
      this.horarios = payload;
      this.updateDom(this.config.animationSpeed);
    } else if (notification === "HORARIOS_ERROR") {
      console.error("Erro ao carregar os horários de ônibus:", payload);
      // Tratar o erro de carregamento dos horários aqui
    }
  },

  // Renderiza o conteúdo do módulo
  getDom: function () {
    var wrapper = document.createElement("div");
    // Verifica se os horários foram carregados corretamente
    if (!this.horarios) {
      wrapper.innerHTML = "Erro ao carregar os horários de ônibus.";
      return wrapper;
    }
    // Obtém o dia da semana atual
    var hoje = new Date().toLocaleDateString("pt", { weekday: "long" });
    // Obtém os horários da linha especificada para o dia atual
    var horariosDoDia = this.horarios[hoje.toLowerCase()][this.config.linha];
    // Cria o conteúdo para exibição
    var content = "<div class='header'>" + hoje + "</div><ul>";
    for (var i = 0; i < horariosDoDia.length; i++) {
      content += "<li>" + horariosDoDia[i] + "</li>";
    }
    content += "</ul>";
    wrapper.innerHTML = content;
    return wrapper;
  }
});
