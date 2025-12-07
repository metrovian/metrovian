document.addEventListener("DOMContentLoaded", () => {
  const STATE_UI = {
    1: { label: "SETUP", class: "setup", disabled: false },
    2: { label: "SYNTHESIS", class: "synthesis", disabled: true },
    3: { label: "PERFORM", class: "perform", disabled: true },
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
    stateCircle.classList.remove(Object.values(STATE_UI).map((v) => v.class));
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
    const res = await fetch("/api?action=read&state=0", {
      method: "GET",
      cache: "no-store",
    });

    if (res.ok == false) {
      applyState(400);
      return;
    }

    const raw = await res.text();
    const state = raw.trim();
    applyState(state);
    return;
  }

  waveConfirm.addEventListener("click", async () => {
    const value = waveSelect?.value || null;
    const valids = Array.from(waveSelect.options).map((opt) => opt.value);
    if (valids.includes(value) == false) {
      console.error("invalid waveform");
      return;
    }

    const res = await fetch(
      `/api?action=write&waveform=${encodeURIComponent(value)}`,
      { method: "GET", cache: "no-store" }
    );

    if (res.ok == false) {
      applyState(400);
      return;
    }

    console.log(`select waveform: ${value}`);
    return;
  });

  updateState();
  setInterval(updateState, 200);
  return;
});
