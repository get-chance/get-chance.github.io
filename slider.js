/*export default*/ class Slider {
    constructor(containerSelector, userOptions = {}) {
        this.container = document.querySelector(containerSelector);
        this.panels = document.querySelectorAll(".panel");
        this.panelsLength = 8; //this.panels.length - 3;
        this.prevButton = document.querySelector(userOptions.navigation.prevEl);
        this.nextButton = document.querySelector(userOptions.navigation.nextEl);

        this.currentIndex = userOptions.initialIndex;
        this.startX = 0;
        this.currentTranslate = 0;
        this.prevTranslate = 0;
        this.isDragging = false;
    }

    updatePosition = () => {
        this.container.style.transform = `translateX(${this.currentTranslate}px)`;
    };

    setPositionByIndex = () => {
        this.currentTranslate = -this.currentIndex * (this.container.offsetWidth + 10); // 10은 panel 사이의 gap /* -은 container 내부의 콘텐츠를 왼쪽으로 이동시키는 효과 */
        this.prevTranslate = this.currentTranslate;
        this.updatePosition();
    };

    touchStart = (e) => {
        this.startX = e.type === "touchstart" ? e.touches[0].clientX : e.clientX;
        this.isDragging = true;
        this.container.style.transition = "none";
    };

    touchMove = (e) => {
        if (!this.isDragging) return; /* 마우스를 클릭하지 않고 그냥 움직이는 경우에는 해당 안함을 뜻함*/
        const currentX = e.type === "touchmove" ? e.touches[0].clientX : e.clientX;
        this.currentTranslate = this.prevTranslate + (currentX - this.startX); /* prevTranslate는 setPositionByIndex에서 지정되는데, 드래그 이전 인덱스에 해당하는 이동을 기억하기 위함인듯 */
        this.updatePosition();
    };

    touchEnd = () => {
        this.isDragging = false;
        const movedBy = this.currentTranslate - this.prevTranslate;

        if (movedBy < -50 && this.currentIndex < this.panelsLength - 1) this.currentIndex++; /* 오른쪽에서 왼쪽으로 드래그 */
        if (movedBy > 50 && this.currentIndex > 0) this.currentIndex--; /* 왼쪽에서 오른쪽으로 드래그 */

        this.setPositionByIndex();
        this.container.style.transition = "transform 0.5s ease-in-out";
    };

    showPrev = () =>  {
        if (this.currentIndex > 0) {
            this.currentIndex--;
            this.setPositionByIndex();
        }
    };

    showNext = () => {
        if (this.currentIndex < this.panelsLength - 1) {
            this.currentIndex++;
            this.setPositionByIndex();
        }
    };

    activate() {
        this.container.addEventListener("mousedown", this.touchStart); /* container에만 이벤트 리스너 추가, 다른 곳 마우스 클릭해도 작동안함 */
        this.container.addEventListener("mousemove", this.touchMove);
        this.container.addEventListener("mouseup", this.touchEnd);
        this.container.addEventListener("mouseleave", () => this.isDragging && this.touchEnd());

        // Touch events
        this.container.addEventListener("touchstart", this.touchStart);
        this.container.addEventListener("touchmove", this.touchMove);
        this.container.addEventListener("touchend", this.touchEnd);

        // Button events
        this.prevButton.addEventListener("click", this.showPrev);
        this.nextButton.addEventListener("click", this.showNext);

        // Initialize
        this.setPositionByIndex();
    }

    deactivate() {
        this.container.removeEventListener("mousedown", this.touchStart); /* container에만 이벤트 리스너 추가, 다른 곳 마우스 클릭해도 작동안함 */
        this.container.removeEventListener("mousemove", this.touchMove);
        this.container.removeEventListener("mouseup", this.touchEnd);
        this.container.removeEventListener("mouseleave", () => this.isDragging && this.touchEnd());

        // Touch events
        this.container.removeEventListener("touchstart", this.touchStart);
        this.container.removeEventListener("touchmove", this.touchMove);
        this.container.removeEventListener("touchend", this.touchEnd);

        // Button events
        this.prevButton.removeEventListener("click", this.showPrev);
        this.nextButton.removeEventListener("click", this.showNext);

        this.container.style.transform = `translateX(0px)`; /* 자바스크립트로 지정한 놈이 css에서 지정한것보다 우선되 자바스크립트로 설정해야 */
    }
}
