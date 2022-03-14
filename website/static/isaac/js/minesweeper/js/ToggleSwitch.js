class ToggleSwitch extends HTMLElement {
  constructor() {
    super();
    this.shadow = this.attachShadow({ mode: 'open' });
    const linkElem = document.createElement('link');
    linkElem.setAttribute('rel', 'stylesheet');
    linkElem.setAttribute('href', './css/toggle_switch.css');
    this.shadow.appendChild(linkElem);
  }

  get checked() {
    return this.input.checked;
  }

  set checked(ischecked) {
    let cur = this.checked;
    if (cur != ischecked) {
      this.input.checked = ischecked;
      let event = new CustomEvent('change', {
        detail: {
          checked: ischecked,
        },
      });
      this.dispatchEvent(event);
    }
  }

  connectedCallback() {
    this.input = document.createElement('input');
    this.input.type = 'checkbox';
    this.input.setAttribute('name', 'toggle-switch');
    this.input.id = this.id + '-input';
    this.input.addEventListener('change', (e) => {
      const event = new CustomEvent('change', {
        detail: { checked: e.target.checked },
      });
      this.dispatchEvent(event);
    });
    this.shadow.appendChild(this.input);
    this.input.addEventListener('change', (e) => {
      this.checked = e.currentTarget.checked;
    });

    let label = document.createElement('label');
    label.className = 'toggle-switch';
    label.setAttribute('for', this.id + '-input');
    this.shadow.appendChild(label);
  }
}

customElements.define('toggle-switch', ToggleSwitch);
