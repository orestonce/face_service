package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"io/ioutil"
	"net/http"
	"net/url"
	"strconv"
	"strings"
)

type FaceUnit struct {
	X            int
	Y            int
	W            int
	H            int
	KeyPointList []Point `json:",omitempty"`
}

type Point struct {
	X int
	Y int
}

func main() {
	var method string
	var localImage string
	var apiHttpUrl string

	flag.StringVar(&apiHttpUrl, "ApiHttpUrl", "http://127.0.0.1:8080", "Api地址")
	flag.StringVar(&method, "Method", "seetaface2-point81", "识别算法: facedetectcnn、seetaface2-point5、seetaface2-point81")
	flag.StringVar(&localImage, "LocalImage", "", "本地图片路径")
	flag.Parse()
	if localImage == `` {
		flag.Usage()
		return
	}
	apiHttpUrl = strings.TrimSuffix(apiHttpUrl, "/")

	values := url.Values{}
	var reqUrl string
	switch method {
	case "facedetectcnn":
		reqUrl = apiHttpUrl + "/facedetectcnn"
	case "seetaface2-point5":
		reqUrl = apiHttpUrl + "/facedetectsf"
		values.Set("use_pts", "5")
	case "seetaface2-point81":
		reqUrl = apiHttpUrl + "/facedetectsf"
		values.Set("use_pts", "81")
	default:
		panic("Unknown method: " + method)
	}
	data, err := ioutil.ReadFile(localImage)
	handleError(err)
	values.Set("image_content", string(data))
	req, err := http.NewRequest("POST", reqUrl, strings.NewReader(values.Encode()))
	handleError(err)
	req.Header.Set("Content-Type", "application/x-www-form-urlencoded")
	resp, err := http.DefaultClient.Do(req)
	handleError(err)
	defer resp.Body.Close()
	data, err = ioutil.ReadAll(resp.Body)
	handleError(err)
	if resp.StatusCode != 200 {
		panic("Call failed, status code " + strconv.Itoa(resp.StatusCode) + ", " + string(data))
	}
	var jsonData []FaceUnit
	err = json.Unmarshal(data, &jsonData)
	handleError(err)
	data, err = json.MarshalIndent(jsonData, "", "    ")
	handleError(err)
	fmt.Println(string(data))
}

func handleError(err error) {
	if err != nil {
		panic(err)
	}
}

