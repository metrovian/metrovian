document.addEventListener("DOMContentLoaded", () => {
  const STATE_UI = {
    1: { label: "SETUP", class: "setup", disabled: false },
    2: { label: "SYNTH", class: "synthesis", disabled: true },
    3: { label: "PLAY", class: "perform", disabled: true },
    400: { label: "ERROR", class: "error", disabled: true },
  };

  const stateCircle = document.getElementById("stateCircle");
  const stateText = document.getElementById("stateText");
  const waveSetup = document.getElementById("waveSetup");
  const waveSelect = document.getElementById("waveSelect");
  const waveConfirm = document.getElementById("waveConfirm");

  function applyState(state) {
    const info = STATE_UI[state];
    stateText.textContent = info.label;
    stateCircle.classList.remove(
      ...Object.values(STATE_UI).map((v) => v.class)
    );

    stateCircle.classList.add(info.class);
    if (info.disabled == false) {
      waveSelect.disabled = false;
      waveConfirm.disabled = false;
      waveSetup.classList.remove("disabled");
    } else {
      waveSelect.disabled = true;
      waveConfirm.disabled = true;
      waveSetup.classList.add("disabled");
    }

    return;
  }

  async function updateState() {
    try {
      const raw = await asyncAPI({ action: "read", state: 0 });
      const state = raw.trim();
      applyState(state);
    } catch (err) {
      applyState(400);
      console.error("State", err);
    }

    return;
  }

  async function updateOptions() {
    const res = await asyncAPI({ action: "read", waveform: 0 });
    const waves = JSON.parse(res);
    waves.forEach((wave) => {
      const opt = document.createElement("option");
      opt.value = wave.id;
      opt.textContent = wave.name;
      waveSelect.appendChild(opt);
    });

    return;
  }

  waveConfirm.addEventListener("click", async () => {
    const value = waveSelect?.value || null;
    const valids = Array.from(waveSelect.options).map((opt) => opt.value);
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
  });

  updateOptions();
  updateState();
  setInterval(updateState, 200);
  return;
});
