async function asyncAPI(params) {
  try {
    const query = new URLSearchParams(params).toString();
    const res = await fetch(`/api?${query}`, {
      method: "GET",
      cache: "no-store",
    });

    if (res.ok == false) {
      throw new Error(`HTTP ${res.status}`);
    }

    return res.text();
  } catch (err) {
    throw err;
  }
}
