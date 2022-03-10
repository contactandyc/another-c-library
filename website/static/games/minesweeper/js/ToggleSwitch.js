class ToggleSwitch extends HTMLElement {
  constructor() {
    super();
    let shadow = this.attachShadow({ mode: 'open' });
    const linkElem = document.createElement('link');
    linkElem.setAttribute('rel', 'stylesheet');
    linkElem.setAttribute('href', './css/toggle_switch.css');
    shadow.appendChild(linkElem);

    let container = document.createElement('label');
    container.className = 'toggle-switch';

    let input = document.createElement('input');
    input.type = 'checkbox';
    container.appendChild(input);

    let slider = document.createElement('span');
    slider.className = 'slider round';
    container.appendChild(slider);
    shadow.appendChild(container);
  }
}

customElements.define('toggle-switch', ToggleSwitch);
