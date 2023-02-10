var array = ["0", "-1000px", "-2000px", "-3000px", "-4000px"];
function imgindex(number) {
  var wrap = document.getElementById("imgs");
  //wrap.style. = array[number];
  wrap.style.transform = "translateX(" + array[number] + ")";
}

var music_array = [
  "昼夜 - 雨的印记",
  "久石让 (ひさいし じょう) - 月光の雲海",
  "Richard Clayderman - 水边的阿狄丽娜",
  "dylanf - 天空之城 (经典钢琴版)",
  "尤大淳 - 婚礼进行曲 (Wedding March)",
  "尤大淳 - 少女的祈祷 (The Maiden’s Prayer)",
  "ClassicalArtists",
  "尤大淳 - 夜曲 (Nocturne)",
  "尤大淳 - 土耳其进行曲 (Turkish March)",
  "尤大淳 - 贝多芬：献给爱丽丝 (For Elise)",
];

function autoPlay() {
  var timer = null;
  var index = 0;
  timer = setInterval(function () {
    if (index == 5) {
      index = 0;
    } else {
      imgindex(index);
      index++;
    }
  }, 2000);
}
autoPlay();

flag = false;

function m_pre(inm) {
  var music = document.getElementById("myAudio");
  music.src = "../static/music/" + music_array[inm] + ".mp3";
  var title = document.getElementsByClassName("music_name")[0];
  title.innerHTML = music_array[inm];
  music.onload();
  music.play();
  flag = true;
}
function m_next(inm) {
  var music = document.getElementById("myAudio");
  music.src = "../static/music/" + music_array[inm] + ".mp3";
  var title = document.getElementsByClassName("music_name")[0];
  title.innerHTML = music_array[inm];
  music.onload();
  music.play();
  flag = true;
}
let inm = 0;
function musicswicth(mflag) {
  if (mflag == true) {
    inm++;
    if (inm == 10) {
      inm = 0;
    }
    m_next(inm);
  } else {
    inm--;
    if (inm == -1) {
      inm = 9;
    }
    m_pre(inm);
  }
}
