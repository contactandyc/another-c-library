class ToggleSwitch extends HTMLElement {
  constructor() {
    super();
    this.shadow = this.attachShadow({ mode: 'open' });
    const linkElem = document.createElement('link');
    linkElem.setAttribute('rel', 'stylesheet');
    linkElem.setAttribute('href', './css/toggle_switch.css');
    this.shadow.appendChild(linkElem);
  }

  connectedCallback() {
    let input = document.createElement('input');
    input.type = 'checkbox';
    input.setAttribute('name', 'toggle-switch');
    input.id = this.id + '-input';
    input.addEventListener('change', (e) => {
      const event = new CustomEvent('change', {
        detail: { checked: e.target.checked },
      });
      this.dispatchEvent(event);
    });
    this.shadow.appendChild(input);
    input.addEventListener('change', (e) => {
      let event = new CustomEvent('change', {
        detail: {
          checked: e.currentTarget.checked,
        },
      });
      this.dispatchEvent(event);
    });

    let label = document.createElement('label');
    label.className = 'toggle-switch';
    label.setAttribute('for', this.id + '-input');
    this.shadow.appendChild(label);
  }
}

customElements.define('toggle-switch', ToggleSwitch);
