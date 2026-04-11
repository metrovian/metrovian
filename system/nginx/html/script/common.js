window.METROVIAN = {
  state: 0,
};

document.addEventListener("DOMContentLoaded", () => {
  const STATE_UI = {
    1: { label: "SETUP", color: "#3b82f6" },
    2: { label: "READY", color: "#22f7b6" },
    18: { label: "SYNTHESIS", color: "#22f7b6" },
    502: { label: "ERROR", color: "#ef4444" },
  };

  const stateText = document.getElementById("stateText");
  const stateLine = document.getElementById("stateLine");
  const pageLinks = document.querySelectorAll(
    'a[href="setup.html"], a[href="system.html"]'
  );

  pageLinks.forEach((link) => {
    link.addEventListener("click", (event) => {
      if (
        event.defaultPrevented ||
        event.button !== 0 ||
        event.metaKey ||
        event.ctrlKey ||
        event.shiftKey ||
        event.altKey
      ) {
        return;
      }

      event.preventDefault();
      document.body.classList.add("page-leaving");
      setTimeout(() => {
        window.location.href = link.href;
      }, 120);
      return;
    });
  });

  async function updateState() {
    const prev = METROVIAN.state;
    try {
      const raw = await asyncAPI({ action: "read", state: 0 });
      const state = raw.trim();
      METROVIAN.state = state;
    } catch (err) {
      METROVIAN.state = 502;
    }

    if (METROVIAN.state != prev) {
      const info = STATE_UI[METROVIAN.state];
      stateText.textContent = info.label;
      stateLine.style.setProperty("--state-color", info.color);
      window.dispatchEvent(
        new CustomEvent("changeState", {
          detail: METROVIAN.state,
        })
      );
    }

    return;
  }

  updateState();
  setInterval(updateState, 200);
  return;
});
