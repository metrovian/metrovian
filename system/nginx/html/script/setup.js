document.addEventListener("DOMContentLoaded", () => {
  const STATE_UI = {
    1: { label: "SETUP", class: "setup" },
    2: { label: "SYNTH", class: "synthesis" },
    3: { label: "PLAY", class: "perform" },
    400: { label: "ERROR", class: "error" },
  };

  const stateCircle = document.getElementById("stateCircle");
  const stateText = document.getElementById("stateText");
  const waveSetup = document.getElementById("waveSetup");
  const waveSelect1 = document.getElementById("waveSelect1");
  const waveSelect2 = document.getElementById("waveSelect2");
  const waveConfirm = document.getElementById("waveConfirm");

  function applyState(state) {
    const info = STATE_UI[state];
    stateText.textContent = info.label;
    stateCircle.classList.remove(
      ...Object.values(STATE_UI).map((v) => v.class)
    );

    stateCircle.classList.add(info.class);
    if (info.label == "SETUP") {
      waveSelect1.disabled = false;
      waveSelect2.disabled = true;
      waveConfirm.disabled = false;
      waveSetup.classList.remove("disabled");
    } else if (info.label == "PLAY") {
      waveSelect1.disabled = true;
      waveSelect2.disabled = false;
      waveConfirm.disabled = false;
      waveSetup.classList.remove("disabled");
    } else {
      waveSelect1.disabled = true;
      waveSelect2.disabled = true;
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
      if (state == 2) {
        const now = await asyncAPI({ action: "read", progress: 0 });
        const max = await asyncAPI({ action: "read", notes: 0 });
        stateCircle.style.setProperty(
          "--progress",
          (parseInt(now) * 100.0) / parseInt(max)
        );
      } else {
        stateCircle.style.removeProperty("--progress");
      }
    } catch (err) {
      applyState(400);
      console.error("State", err);
    }

    return;
  }

  async function updateOptions() {
    const res = await asyncAPI({ action: "read", waveforms: 0 });
    const waves = JSON.parse(res);
    waves.forEach((wave) => {
      const opt = document.createElement("option");
      opt.value = wave.id;
      opt.textContent = wave.name;
      waveSelect1.appendChild(opt);
    });

    if (stateText.textContent == "SYNTH" || stateText.textContent == "PLAY") {
      const select = await asyncAPI({ action: "read", waveform: 0 });
      const id = String(select);
      waveSelect1.value = id;
    }

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

  updateState();
  updateOptions();
  setInterval(updateState, 200);
  return;
});
