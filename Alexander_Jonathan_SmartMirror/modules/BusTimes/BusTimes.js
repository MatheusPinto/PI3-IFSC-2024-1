/* global Module */

Module.register("BusTimes", {
  defaults: {
    jsonFile: "modules/BusTimes/bus_times.json", // Caminho para o arquivo JSON com os horários
    updateInterval: 60000, // Atualização a cada 60 segundos
    timeFormat: 24         // Formato de hora: 24 ou 12
  },

  requiresVersion: "2.1.0",

  start: function() {
    console.log("Módulo BusTimes iniciado");
    this.loaded = false;
    this.getBusTimes(); // Chama imediatamente para verificar se funciona
    this.scheduleUpdate();
  },

  // Agenda a atualização para verificar os horários
  scheduleUpdate: function() {
    const self = this;
    setInterval(() => {
      console.log("Atualizando horários dos ônibus");
      self.getBusTimes();
    }, this.config.updateInterval);
  },

  // Lê o arquivo JSON e processa os dados
  getBusTimes: function() {
    console.log("Chamando getBusTimes");

    const self = this;

    fetch(this.config.jsonFile)
      .then(response => response.json())
      .then(data => {
        self.processData(data);
      })
      .catch(error => {
        console.error("Erro ao ler o arquivo JSON:", error);
        self.processData({}); // Passa um objeto vazio em caso de erro
      });
  },

  // Processa os dados JSON e atualiza o DOM
  processData: function(data) {
    console.log("Dados recebidos:", data); // Verifique os dados recebidos

    const self = this;
    const now = new Date();
    const day = this.getDayOfWeek(now); // Pega o dia da semana
    const currentTime = this.formatTime(now);

    console.log("Dia da semana:", day);
    console.log("Hora atual:", currentTime);

    const nextBusesIda = this.getNextBusTimes(data.ida.horarios[day] || [], currentTime);
    const nextBusesRetorno = this.getNextBusTimes(data.retorno.horarios[day] || [], currentTime);

    console.log("Próximos ônibus ida:", nextBusesIda);
    console.log("Próximos ônibus retorno:", nextBusesRetorno);

    this.busInfoIda = {
      nome: data.ida.nome || "Nome não disponível",
      origem: data.ida.origem || "Origem não disponível",
      tarifa: data.ida.tarifa || "Tarifa não disponível",
      nextBusTimes: nextBusesIda
    }; // Guarda as informações dos próximos ônibus de ida

    this.busInfoRetorno = {
      nome: data.retorno.nome || "Nome não disponível",
      origem: data.retorno.origem || "Origem não disponível",
      tarifa: data.retorno.tarifa || "Tarifa não disponível",
      nextBusTimes: nextBusesRetorno
    }; // Guarda as informações dos próximos ônibus de retorno

    this.loaded = true;
    this.updateDom();
  },

  // Retorna o dia da semana em minúsculas (segunda, terca, ...)
  getDayOfWeek: function(date) {
    const days = ["domingo", "segunda", "terca", "quarta", "quinta", "sexta", "sabado"];
    return days[date.getDay()];
  },

  // Formata o tempo no formato especificado (24 ou 12 horas)
  formatTime: function(date) {
    let hours = date.getHours();
    const minutes = date.getMinutes();
    if (this.config.timeFormat === 12) {
      const ampm = hours >= 12 ? "PM" : "AM";
      hours = hours % 12 || 12; // Converte para formato 12h
      return `${hours}:${minutes < 10 ? '0' + minutes : minutes} ${ampm}`;
    } else {
      return `${hours < 10 ? '0' + hours : hours}:${minutes < 10 ? '0' + minutes : minutes}`;
    }
  },

  // Retorna os próximos dois horários de ônibus
  getNextBusTimes: function(times, currentTime) {
    console.log("Horários disponíveis:", times);
    console.log("Hora atual:", currentTime);

    const nextBuses = times.filter(time => time > currentTime).slice(0, 2);

    console.log("Próximos ônibus encontrados:", nextBuses);

    return nextBuses.length > 0 ? nextBuses : ["Nenhum ônibus disponível hoje"];
  },

  // Cria o conteúdo para exibir no módulo
  getDom: function() {
    const wrapper = document.createElement("div");

    if (!this.loaded) {
      const loading = document.createElement("div");
      loading.innerHTML = "Carregando horários...";
      loading.className = "loading";
      wrapper.appendChild(loading);
      return wrapper;
    }

    // Exibe as informações de ida
    const headerIda = document.createElement("h2");
    headerIda.innerHTML = "Horários de Ida";
    wrapper.appendChild(headerIda);

    const busInfoIda = document.createElement("div");
    busInfoIda.className = "bus-info";
    busInfoIda.innerHTML = `
      Ônibus: ${this.busInfoIda.nome}<br>
      Origem: ${this.busInfoIda.origem}<br>
      Tarifa: ${this.busInfoIda.tarifa}<br>
      Próximos horários: ${this.busInfoIda.nextBusTimes.join(", ")}
    `;
    wrapper.appendChild(busInfoIda);

    // Exibe as informações de retorno
    const headerRetorno = document.createElement("h2");
    headerRetorno.innerHTML = "Horários de Retorno";
    wrapper.appendChild(headerRetorno);

    const busInfoRetorno = document.createElement("div");
    busInfoRetorno.className = "bus-info";
    busInfoRetorno.innerHTML = `
      Ônibus: ${this.busInfoRetorno.nome}<br>
      Origem: ${this.busInfoRetorno.origem}<br>
      Tarifa: ${this.busInfoRetorno.tarifa}<br>
      Próximos horários: ${this.busInfoRetorno.nextBusTimes.join(", ")}
    `;
    wrapper.appendChild(busInfoRetorno);

    return wrapper;
  },

  getStyles: function () {
    return [
      "BusTimes.css"
    ];
  }
});
