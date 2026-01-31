document.addEventListener("DOMContentLoaded", () => {
  window.addEventListener("changeState", async (event) => {
    const STATE_UI = {
      1: {
        waveSelect1: false,
        waveSelect2: true,
        waveConfirm: false,
      },
      3: {
        waveSelect1: true,
        waveSelect2: false,
        waveConfirm: false,
      },
    };

    const STATE_DEFAULT = {
      waveSelect1: true,
      waveSelect2: true,
      waveConfirm: true,
    };

    const ui = STATE_UI[event.detail] ?? STATE_DEFAULT;
    waveSelect1.disabled = ui.waveSelect1;
    waveSelect2.disabled = ui.waveSelect2;
    waveConfirm.disabled = ui.waveConfirm;
    waveSelect1.classList.toggle("disabled", ui.waveSelect1);
    waveSelect2.classList.toggle("disabled", ui.waveSelect2);
    waveConfirm.classList.toggle("disabled", ui.waveConfirm);

    if (event.detail == 2 || event.detail == 3) {
      const select = await asyncAPI({ action: "read", waveform: 0 });
      const id = String(select);
      waveSelect1.value = id;
    }

    return;
  });

  const waveSelect1 = document.getElementById("waveSelect1");
  const waveSelect2 = document.getElementById("waveSelect2");
  const waveConfirm = document.getElementById("waveConfirm");

  async function updateOptions() {
    const res = await asyncAPI({ action: "read", waveforms: 0 });
    const waves = JSON.parse(res);
    waves.forEach((wave) => {
      const opt = document.createElement("option");
      opt.value = wave.id;
      opt.textContent = wave.name;
      waveSelect1.appendChild(opt);
    });

    return;
  }

  waveConfirm.addEventListener("click", async () => {
    if (stateText.textContent == "SETUP") {
      const value = waveSelect1?.value || null;
      const valids = Array.from(waveSelect1.options).map((opt) => opt.value);
      if (valids.includes(value) == false) {
        console.error("Invalid Waveform");
        return;
      }

      try {
        await asyncAPI({ action: "write", waveform: value });
      } catch (err) {
        console.warn(`Select Waveform: ${value}`);
        return;
      }

      console.log(`Select Waveform: ${value}`);
      return;
    } else if (stateText.textContent == "PLAY") {
      const value = waveSelect2?.value || null;
      const valids = Array.from(waveSelect2.options).map((opt) => opt.value);
      if (valids.includes(value) == false) {
        console.error("Invalid MIDI");
        return;
      }
    }
  });

  updateOptions();
  return;
});
