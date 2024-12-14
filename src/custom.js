const swiper = new Swiper('.swiper', {
    /*
    // Optional parameters
    direction: 'vertical',
    loop: true,
    */

    // If we need pagination
    pagination: {
      el: '.swiper-pagination',
    },
  
    // Navigation arrows
    navigation: {
      nextEl: '.swiper-button-next',
      prevEl: '.swiper-button-prev',
    },

    on: {
        activeIndexChange: function () {
            const descriptions = [
                { title: 'HBM3E :', text: '8단 HBM 고속 메모리' },
                { title: 'Exynos :', text: '삼성전자 설계 AP' },
                { title: 'Foundary Factory :', text: '5 ~ 3nm 최첨단 공정' }
            ];
            console.log(this.realIndex+'번째 slide입니다.');
            $('.htit').text(descriptions[this.realIndex].title);
            $('.htxt').text(descriptions[this.realIndex].text);
        }
    }
  
    /*
    // And if we need scrollbar
    scrollbar: {
      el: '.swiper-scrollbar',
    },
    */
  });